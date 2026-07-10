#include "optim_cuda.h"
#include "common.cuh"
#include <cooperative_groups.h>

namespace cg = cooperative_groups;

// ============================================================================
// Learning Rate Schedule Helpers (Device)
// ============================================================================

__device__ float sneppx_lr_cosine(
    int step, int warmup_steps, int total_steps,
    float lr_init, float lr_min
) {
    if (step < warmup_steps) {
        return lr_init * (float)(step + 1) / (float)(warmup_steps + 1);
    }
    float progress = (float)(step - warmup_steps) / (float)(total_steps - warmup_steps);
    float cosine = 0.5f * (1.0f + cosf(M_PI * progress));
    return lr_min + (lr_init - lr_min) * cosine;
}

__device__ float sneppx_lr_linear(
    int step, int warmup_steps, int total_steps,
    float lr_init, float lr_min
) {
    if (step < warmup_steps) {
        return lr_init * (float)(step + 1) / (float)(warmup_steps + 1);
    }
    float progress = (float)(step - warmup_steps) / (float)(total_steps - warmup_steps);
    return lr_init - (lr_init - lr_min) * progress;
}

__device__ float sneppx_lr_constant_warmup(int step, int warmup_steps, float lr_init) {
    if (step < warmup_steps) {
        return lr_init * (float)(step + 1) / (float)(warmup_steps + 1);
    }
    return lr_init;
}

// ============================================================================
// SGD Step
// ============================================================================

__global__ void sgd_step_kernel(
    half* params,
    const half* grads,
    float lr,
    float weight_decay,
    int numel
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= numel) return;
    
    float p = __half2float(params[idx]);
    float g = __half2float(grads[idx]);
    
    p -= lr * (g + weight_decay * p);
    
    params[idx] = __float2half_rn(p);
}

SNEPPX_CudaError sneppx_cuda_sgd_step(
    SNEPPX_CudaStream_t stream,
    half* params,
    const half* grads,
    float lr,
    float weight_decay,
    int numel
) {
    if (!params || !grads) return SNEPPX_CUDA_ERROR_INVALID_ARG;
    
    int block = 256;
    int grid = (numel + block - 1) / block;
    
    sgd_step_kernel<<<grid, block, 0, stream>>>(params, grads, lr, weight_decay, numel);
    
    cudaError_t err = cudaGetLastError();
    return (err == cudaSuccess) ? SNEPPX_CUDA_SUCCESS : SNEPPX_CUDA_ERROR_LAUNCH_FAILED;
}

// ============================================================================
// SGD with Momentum
// ============================================================================

__global__ void sgd_momentum_step_kernel(
    half* params,
    const half* grads,
    float* momentum,
    float lr,
    float momentum_factor,
    float dampening,
    float weight_decay,
    bool nesterov,
    int numel
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= numel) return;
    
    float p = __half2float(params[idx]);
    float g = __half2float(grads[idx]);
    float m = momentum[idx];
    
    // Apply weight decay
    g += weight_decay * p;
    
    // Update momentum
    m = momentum_factor * m + (1.0f - dampening) * g;
    momentum[idx] = m;
    
    // Update parameters
    if (nesterov) {
        p -= lr * (g + momentum_factor * m);
    } else {
        p -= lr * m;
    }
    
    params[idx] = __float2half_rn(p);
}

SNEPPX_CudaError sneppx_cuda_sgd_momentum_step(
    SNEPPX_CudaStream_t stream,
    half* params,
    const half* grads,
    float* momentum,
    float lr,
    float momentum_factor,
    float dampening,
    float weight_decay,
    bool nesterov,
    int numel
) {
    if (!params || !grads || !momentum) return SNEPPX_CUDA_ERROR_INVALID_ARG;
    
    int block = 256;
    int grid = (numel + block - 1) / block;
    
    sgd_momentum_step_kernel<<<grid, block, 0, stream>>>(
        params, grads, momentum, lr, momentum_factor,
        dampening, weight_decay, nesterov, numel
    );
    
    cudaError_t err = cudaGetLastError();
    return (err == cudaSuccess) ? SNEPPX_CUDA_SUCCESS : SNEPPX_CUDA_ERROR_LAUNCH_FAILED;
}

// ============================================================================
// AdamW Step (fused)
// ============================================================================

__global__ void adamw_step_kernel(
    half* params,
    const half* grads,
    float* exp_avg,
    float* exp_avg_sq,
    int step,
    float lr,
    float beta1,
    float beta2,
    float epsilon,
    float weight_decay,
    int numel
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= numel) return;
    
    float p = __half2float(params[idx]);
    float g = __half2float(grads[idx]);
    
    float m = exp_avg[idx];
    float v = exp_avg_sq[idx];
    
    // Update biased moments
    m = beta1 * m + (1.0f - beta1) * g;
    v = beta2 * v + (1.0f - beta2) * g * g;
    
    exp_avg[idx] = m;
    exp_avg_sq[idx] = v;
    
    // Bias correction
    float m_hat = m / (1.0f - powf(beta1, (float)step));
    float v_hat = v / (1.0f - powf(beta2, (float)step));
    
    // Decoupled weight decay (AdamW)
    p -= lr * weight_decay * p;
    
    // Update
    p -= lr * m_hat / (sqrtf(v_hat) + epsilon);
    
    params[idx] = __float2half_rn(p);
}

SNEPPX_CudaError sneppx_cuda_adamw_step(
    SNEPPX_CudaStream_t stream,
    half* params,
    const half* grads,
    float* exp_avg,
    float* exp_avg_sq,
    int step,
    float lr,
    float beta1,
    float beta2,
    float epsilon,
    float weight_decay,
    int numel
) {
    if (!params || !grads || !exp_avg || !exp_avg_sq) {
        return SNEPPX_CUDA_ERROR_INVALID_ARG;
    }
    
    int block = 256;
    int grid = (numel + block - 1) / block;
    
    adamw_step_kernel<<<grid, block, 0, stream>>>(
        params, grads, exp_avg, exp_avg_sq, step,
        lr, beta1, beta2, epsilon, weight_decay, numel
    );
    
    cudaError_t err = cudaGetLastError();
    return (err == cudaSuccess) ? SNEPPX_CUDA_SUCCESS : SNEPPX_CUDA_ERROR_LAUNCH_FAILED;
}

// ============================================================================
// Lion Step (fused)
// ============================================================================

__global__ void lion_step_kernel(
    half* params,
    const half* grads,
    float* exp_avg,
    int step,
    float lr,
    float beta1,
    float beta2,
    float weight_decay,
    int numel
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= numel) return;
    
    float p = __half2float(params[idx]);
    float g = __half2float(grads[idx]);
    float m = exp_avg[idx];
    
    // Lion: update = sign(m * beta1 + g * (1 - beta1))
    float update = m * beta1 + g * (1.0f - beta1);
    float sign = (update > 0.0f) ? 1.0f : ((update < 0.0f) ? -1.0f : 0.0f);
    
    // Update momentum (use current gradient)
    exp_avg[idx] = beta2 * m + (1.0f - beta2) * g;
    
    // Weight decay
    p -= lr * weight_decay * p;
    
    // Update
    p -= lr * sign;
    
    params[idx] = __float2half_rn(p);
}

SNEPPX_CudaError sneppx_cuda_lion_step(
    SNEPPX_CudaStream_t stream,
    half* params,
    const half* grads,
    float* exp_avg,
    int step,
    float lr,
    float beta1,
    float beta2,
    float weight_decay,
    int numel
) {
    if (!params || !grads || !exp_avg) return SNEPPX_CUDA_ERROR_INVALID_ARG;
    
    int block = 256;
    int grid = (numel + block - 1) / block;
    
    lion_step_kernel<<<grid, block, 0, stream>>>(
        params, grads, exp_avg, step,
        lr, beta1, beta2, weight_decay, numel
    );
    
    cudaError_t err = cudaGetLastError();
    return (err == cudaSuccess) ? SNEPPX_CUDA_SUCCESS : SNEPPX_CUDA_ERROR_LAUNCH_FAILED;
}

// ============================================================================
// LAMB Step (Layer-wise Adaptive Moments)
// ============================================================================

__global__ void lamb_step_kernel(
    half* params,
    const half* grads,
    float* exp_avg,
    float* exp_avg_sq,
    int step,
    float lr,
    float beta1,
    float beta2,
    float epsilon,
    float weight_decay,
    int numel
) {
    extern __shared__ float smem[];
    
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int tid = threadIdx.x;
    
    float p = __half2float(params[idx]);
    float g = __half2float(grads[idx]);
    
    float m = exp_avg[idx];
    float v = exp_avg_sq[idx];
    
    m = beta1 * m + (1.0f - beta1) * g;
    v = beta2 * v + (1.0f - beta2) * g * g;
    
    exp_avg[idx] = m;
    exp_avg_sq[idx] = v;
    
    float m_hat = m / (1.0f - powf(beta1, (float)step));
    float v_hat = v / (1.0f - powf(beta2, (float)step));
    
    float update = m_hat / (sqrtf(v_hat) + epsilon);
    update += weight_decay * p;
    
    // Compute trust ratio
    float p_norm = fabsf(p);
    float g_norm = fabsf(update);
    
    float trust_ratio = 1.0f;
    if (p_norm > 0.0f && g_norm > 0.0f) {
        trust_ratio = p_norm / g_norm;
    }
    
    // Warp-level trust ratio (take max)
    if (tid < numel) {
        p -= lr * trust_ratio * update;
        params[idx] = __float2half_rn(p);
    }
}

SNEPPX_CudaError sneppx_cuda_lamb_step(
    SNEPPX_CudaStream_t stream,
    half* params,
    const half* grads,
    float* exp_avg,
    float* exp_avg_sq,
    int step,
    float lr,
    float beta1,
    float beta2,
    float epsilon,
    float weight_decay,
    int numel
) {
    if (!params || !grads || !exp_avg || !exp_avg_sq) {
        return SNEPPX_CUDA_ERROR_INVALID_ARG;
    }
    
    int block = 256;
    int grid = (numel + block - 1) / block;
    
    lamb_step_kernel<<<grid, block, 0, stream>>>(
        params, grads, exp_avg, exp_avg_sq, step,
        lr, beta1, beta2, epsilon, weight_decay, numel
    );
    
    cudaError_t err = cudaGetLastError();
    return (err == cudaSuccess) ? SNEPPX_CUDA_SUCCESS : SNEPPX_CUDA_ERROR_LAUNCH_FAILED;
}

// ============================================================================
// LARS Step
// ============================================================================

__global__ void lars_step_kernel(
    half* params,
    const half* grads,
    float* momentum,
    int step,
    float lr,
    float momentum_factor,
    float weight_decay,
    float epsilon,
    float trust_coefficient,
    int numel
) {
    extern __shared__ float smem[];
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    
    float p_norm_local = 0.0f;
    float g_norm_local = 0.0f;
    
    int start = bid * blockDim.x;
    int end = min(start + blockDim.x, numel);
    
    for (int i = start + tid; i < end; i += blockDim.x) {
        float p = __half2float(params[i]);
        float g = __half2float(grads[i]);
        p_norm_local += p * p;
        g_norm_local += g * g;
    }
    
    // Warp reduce
    p_norm_local = sqrtf(sneppx_warp_reduce_sum(p_norm_local));
    g_norm_local = sqrtf(sneppx_warp_reduce_sum(g_norm_local));
    
    if (tid == 0) {
        float lr_scale = 1.0f;
        if (p_norm_local > 0.0f && g_norm_local > 0.0f) {
            lr_scale = trust_coefficient * p_norm_local / (g_norm_local + weight_decay * p_norm_local + epsilon);
        }
        smem[bid] = lr_scale;
    }
    __syncthreads();
    
    // Apply update (simplified - use global lr_scale)
    for (int i = start + tid; i < end; i += blockDim.x) {
        float p = __half2float(params[i]);
        float g = __half2float(grads[i]);
        float m = momentum[i];
        
        g += weight_decay * p;
        m = momentum_factor * m + g;
        momentum[i] = m;
        
        p -= lr * smem[bid] * m;
        params[i] = __float2half_rn(p);
    }
}

SNEPPX_CudaError sneppx_cuda_lars_step(
    SNEPPX_CudaStream_t stream,
    half* params,
    const half* grads,
    float* momentum,
    int step,
    float lr,
    float momentum_factor,
    float weight_decay,
    float epsilon,
    float trust_coefficient,
    int numel
) {
    if (!params || !grads || !momentum) return SNEPPX_CUDA_ERROR_INVALID_ARG;
    
    int block = 256;
    int grid = (numel + block - 1) / block;
    
    lars_step_kernel<<<grid, block, 0, stream>>>(
        params, grads, momentum, step,
        lr, momentum_factor, weight_decay,
        epsilon, trust_coefficient, numel
    );
    
    cudaError_t err = cudaGetLastError();
    return (err == cudaSuccess) ? SNEPPX_CUDA_SUCCESS : SNEPPX_CUDA_ERROR_LAUNCH_FAILED;
}

// ============================================================================
// AdaFactor Step (Matrix factorization of second moment)
// ============================================================================

__global__ void adafactor_step_kernel(
    half* params,
    const half* grads,
    float* exp_avg_sq,
    float* exp_avg_sq_row,
    float* exp_avg_sq_col,
    int step,
    float lr,
    float beta2,
    float epsilon,
    float weight_decay,
    int numel,
    int rows,
    int cols
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= numel) return;
    
    int col = idx % cols;
    int row = idx / cols;
    
    float p = __half2float(params[idx]);
    float g = __half2float(grads[idx]);
    
    float v = exp_avg_sq[idx];
    float vr = exp_avg_sq_row[row];
    float vc = exp_avg_sq_col[col];
    
    // Update factored second moments
    float g2 = g * g;
    float beta2t = beta2;
    
    v = beta2t * v + (1.0f - beta2t) * g2;
    vr = beta2t * vr + (1.0f - beta2t) * g2 / cols;
    vc = beta2t * vc + (1.0f - beta2t) * g2 / rows;
    
    exp_avg_sq[idx] = v;
    exp_avg_sq_row[row] = vr;
    exp_avg_sq_col[col] = vc;
    
    // Compute adaptive step
    float rms = sqrtf(vr * vc / fmaxf(v, 1e-10f));
    float update = g / (rms + epsilon);
    
    // Weight decay
    update += weight_decay * p;
    
    // Bias correction for step
    float step_num = (float)step;
    float bias_corr = 1.0f / (1.0f - powf(beta2, step_num));
    update *= bias_corr;
    
    p -= lr * update;
    params[idx] = __float2half_rn(p);
}

SNEPPX_CudaError sneppx_cuda_adafactor_step(