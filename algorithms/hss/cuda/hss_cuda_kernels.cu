/*
 * HSS CUDA Kernels Implementation — v1.0
 * GPU-accelerated hierarchical softmax, sparse-dense matmul, and top-k.
 *
 * DEPENDENCIES: CUDA Toolkit 11.0+
 * Build with: nvcc -arch=sm_70 -c hss_cuda_kernels.cu
 */

#include "hss_cuda_kernels.cuh"
#include <cuda_runtime.h>
#include <float.h>

/* ---------- Hierarchical Softmax Kernel ---------- */

__global__ void hss_softmax_kernel(const float* input, const int* tree_indices,
                                    float* output, int batch_size, int num_classes,
                                    int tree_depth) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= batch_size * num_classes) return;

    int batch = idx / num_classes;
    int cls = idx % num_classes;
    int tree_node = tree_indices ? tree_indices[cls] : cls;

    float val = input[idx];
    float max_val = -FLT_MAX;

    /* Find max within this batch row's tree group for numerical stability */
    int group_start = batch * num_classes;
    int group_end = group_start + num_classes;
    for (int i = group_start + threadIdx.x; i < group_end; i += blockDim.x) {
        float v = input[i];
        if (v > max_val) max_val = v;
    }

    float sum = 0.0f;
    for (int i = group_start + threadIdx.x; i < group_end; i += blockDim.x) {
        sum += expf(input[i] - max_val);
    }

    output[idx] = expf(val - max_val) / (sum + 1e-10f);
}

void launch_hss_softmax_kernel(cudaStream_t stream,
                                const float* input, const int* tree_indices,
                                float* output, int batch_size, int num_classes,
                                int tree_depth) {
    int total = batch_size * num_classes;
    int threads = 256;
    int blocks = (total + threads - 1) / threads;
    hss_softmax_kernel<<<blocks, threads, 0, stream>>>(
        input, tree_indices, output, batch_size, num_classes, tree_depth);
}

/* ---------- Sparse-Dense Matmul Kernel (top-k experts) ---------- */

__global__ void hss_sparse_matmul_kernel(const float* sparse_weights,
                                          const int* indices,
                                          const float* dense_input, float* output,
                                          int batch_size, int num_experts,
                                          int expert_size, int k) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int total = batch_size * k * expert_size;
    if (tid >= total) return;

    int out_idx = tid;
    int es = tid % expert_size;
    int tmp = tid / expert_size;
    int ek = tmp % k;
    int b = tmp / k;

    int expert_idx = indices[b * k + ek];
    if (expert_idx < 0 || expert_idx >= num_experts) {
        output[out_idx] = 0.0f;
        return;
    }

    float sum = 0.0f;
    int input_dim = expert_size; /* assumes input_dim == expert_size for simplicity */
    for (int i = 0; i < input_dim; i++) {
        sum += sparse_weights[expert_idx * expert_size * input_dim + es * input_dim + i] *
               dense_input[b * input_dim + i];
    }
    output[out_idx] = sum;
}

void launch_hss_sparse_matmul(cudaStream_t stream,
                               const float* sparse_weights, const int* indices,
                               const float* dense_input, float* output,
                               int batch_size, int num_experts, int expert_size,
                               int k) {
    int total = batch_size * k * expert_size;
    int threads = 256;
    int blocks = (total + threads - 1) / threads;
    hss_sparse_matmul_kernel<<<blocks, threads, 0, stream>>>(
        sparse_weights, indices, dense_input, output,
        batch_size, num_experts, expert_size, k);
}

/* ---------- Top-K Selection Kernel ---------- */

__global__ void hss_topk_kernel(const float* scores, int* indices,
                                 float* values, int batch_size,
                                 int num_classes, int k) {
    extern __shared__ float shared_scores[];
    int* shared_indices = (int*)&shared_scores[num_classes];

    int batch = blockIdx.x;
    int base = batch * num_classes;

    /* Load scores into shared memory */
    for (int i = threadIdx.x; i < num_classes; i += blockDim.x) {
        shared_scores[i] = scores[base + i];
        shared_indices[i] = i;
    }
    __syncthreads();

    /* Simple iterative top-k: bubble smallest k to the end */
    for (int round = 0; round < k; round++) {
        int last = num_classes - 1 - round;
        for (int i = threadIdx.x; i < last; i += blockDim.x) {
            if (shared_scores[i] > shared_scores[i + 1]) {
                float tmp_s = shared_scores[i];
                shared_scores[i] = shared_scores[i + 1];
                shared_scores[i + 1] = tmp_s;
                int tmp_i = shared_indices[i];
                shared_indices[i] = shared_indices[i + 1];
                shared_indices[i + 1] = tmp_i;
            }
        }
        __syncthreads();
    }

    /* Write top-k results (largest k are at positions num_classes-k .. num_classes-1) */
    int out_base = batch * k;
    for (int i = threadIdx.x; i < k; i += blockDim.x) {
        int src = num_classes - k + i;
        values[out_base + i] = shared_scores[src];
        indices[out_base + i] = shared_indices[src];
    }
}

void launch_hss_topk(cudaStream_t stream,
                      const float* scores, int* indices, float* values,
                      int batch_size, int num_classes, int k) {
    int shared_bytes = (num_classes * sizeof(float)) + (num_classes * sizeof(int));
    hss_topk_kernel<<<batch_size, 256, shared_bytes, stream>>>(
        scores, indices, values, batch_size, num_classes, k);
}

/* ---------- Parallel Prefix Scan for SSM (Blelloch) ---------- */

__global__ void hss_blelloch_scan_kernel(const float* A_bar, const float* B_bar,
                                          const float* C, const float* D,
                                          const float* x_seq,
                                          float* h_seq, float* y_seq,
                                          int seq_len, int s_dim, int i_dim, int o_dim) {
    __shared__ float pair_A[1024 * 64]; /* max s_dim=64, seq_len up to 1024 */
    __shared__ float pair_b[1024 * 64];

    int t = threadIdx.x;
    int total = seq_len * s_dim;

    /* Phase 1: initialise pairs */
    if (t < seq_len) {
        for (int i = 0; i < s_dim; i++) {
            pair_A[t * s_dim + i] = A_bar[t * s_dim + i];
            pair_b[t * s_dim + i] = 0.0f;
        }
    }
    /* Compute b_t = B_bar @ x_t */
    if (t < seq_len * s_dim) {
        int seq_idx = t / s_dim;
        int s_idx = t % s_dim;
        float sum = 0.0f;
        for (int k = 0; k < i_dim; k++) {
            sum += B_bar[s_idx * i_dim + k] * x_seq[seq_idx * i_dim + k];
        }
        pair_b[t] = sum;
    }
    __syncthreads();

    /* Phase 2: up-sweep (Blelloch tree) */
    for (int stride = 1; stride < seq_len; stride *= 2) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        int right = (idx + 1) * stride * 2 - 1;
        int left = right - stride;
        if (right < seq_len && idx < seq_len / (stride * 2)) {
            /* combine right = right o left */
            for (int i = 0; i < s_dim; i++) {
                float sum_b = 0.0f;
                for (int j = 0; j < s_dim; j++) {
                    sum_b += pair_A[right * s_dim + i * s_dim + j] * pair_b[left * s_dim + j];
                }
                pair_b[right * s_dim + i] = sum_b + pair_b[right * s_dim + i];
            }
            float tmp_A[64];
            for (int i = 0; i < s_dim; i++) {
                for (int j = 0; j < s_dim; j++) {
                    float sum = 0.0f;
                    for (int k = 0; k < s_dim; k++) {
                        sum += pair_A[right * s_dim + i * s_dim + k] * pair_A[left * s_dim + k * s_dim + j];
                    }
                    tmp_A[i * s_dim + j] = sum;
                }
            }
            for (int i = 0; i < s_dim * s_dim; i++) {
                pair_A[right * s_dim + i] = tmp_A[i];
            }
        }
        __syncthreads();
    }

    /* Phase 3: down-sweep */
    if (t == 0 && seq_len > 0) {
        int last = seq_len - 1;
        for (int i = 0; i < s_dim * s_dim; i++) pair_A[last * s_dim + i] = 0.0f;
        for (int i = 0; i < s_dim; i++) pair_A[last * s_dim + i * s_dim + i] = 1.0f;
        for (int i = 0; i < s_dim; i++) pair_b[last * s_dim + i] = 0.0f;
    }
    __syncthreads();

    for (int stride = seq_len / 2; stride > 0; stride /= 2) {
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        int right = (idx + 1) * stride * 2 - 1;
        int left = right - stride;
        if (right < seq_len && idx < seq_len / (stride * 2)) {
            float tmp_A[64];
            for (int i = 0; i < s_dim; i++) {
                for (int j = 0; j < s_dim; j++) {
                    float sum = 0.0f;
                    for (int k = 0; k < s_dim; k++) {
                        sum += pair_A[right * s_dim + i * s_dim + k] * pair_A[left * s_dim + k * s_dim + j];
                    }
                    tmp_A[i * s_dim + j] = sum;
                }
            }
            for (int i = 0; i < s_dim * s_dim; i++) {
                pair_A[left * s_dim + i] = tmp_A[i];
            }
            for (int i = 0; i < s_dim; i++) {
                float sum = 0.0f;
                for (int j = 0; j < s_dim; j++) {
                    sum += pair_A[right * s_dim + i * s_dim + j] * pair_b[left * s_dim + j];
                }
                pair_b[left * s_dim + i] = sum + pair_b[right * s_dim + i];
            }
        }
        __syncthreads();
    }

    /* Extract hidden states and compute outputs */
    for (int t_idx = threadIdx.x; t_idx < seq_len; t_idx += blockDim.x) {
        float h_t[64];
        float h_prev[64] = {0.0f};
        if (t_idx > 0) {
            for (int i = 0; i < s_dim; i++) h_prev[i] = h_seq[(t_idx - 1) * s_dim + i];
        }
        for (int i = 0; i < s_dim; i++) {
            float sum = 0.0f;
            for (int j = 0; j < s_dim; j++) {
                sum += pair_A[t_idx * s_dim + i * s_dim + j] * h_prev[j];
            }
            h_t[i] = sum + pair_b[t_idx * s_dim + i];
        }
        for (int i = 0; i < s_dim; i++) h_seq[t_idx * s_dim + i] = h_t[i];
        for (int i = 0; i < o_dim; i++) {
            float y = 0.0f;
            for (int j = 0; j < s_dim; j++) y += C[i * s_dim + j] * h_t[j];
            for (int k = 0; k < i_dim; k++) y += D[i * i_dim + k] * x_seq[t_idx * i_dim + k];
            y_seq[t_idx * o_dim + i] = y;
        }
    }
}

void launch_hss_parallel_scan(cudaStream_t stream,
                               const float* A_bar, const float* B_bar,
                               const float* C, const float* D,
                               const float* x_seq, float* h_seq, float* y_seq,
                               int seq_len, int s_dim, int i_dim, int o_dim) {
    int threads = 256;
    int blocks = (seq_len + threads - 1) / threads;
    hss_blelloch_scan_kernel<<<blocks, threads, 0, stream>>>(
        A_bar, B_bar, C, D, x_seq, h_seq, y_seq, seq_len, s_dim, i_dim, o_dim);
}
