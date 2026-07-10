#ifndef SNEPPX_SIMD_GEMM_H
#define SNEPPX_SIMD_GEMM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * SIMD Dispatch Layer for Tensor Engine
 *
 * Provides CPU GEMM / elementwise / reduction kernels with runtime
 * dispatch across instruction-set levels (Scalar, SSE4.2, AVX2, AVX-512).
 * All kernels operate on contiguous float32 row-major matrices unless
 * noted. Blocking + prefetch keep L1/L2 hot for very large matmuls.
 * ========================================================================= */

typedef enum {
    SNEPPX_SIMD_SCALAR = 0,
    SNEPPX_SIMD_SSE42  = 1,
    SNEPPX_SIMD_AVX2   = 2,
    SNEPPX_SIMD_AVX512 = 3
} SNEPPXSimdLevel;

/* Detect the highest SIMD level available at runtime */
SNEPPXSimdLevel SNEPPX_simd_detect(void);

/* Human-readable string for a SIMD level */
const char* SNEPPX_simd_level_name(SNEPPXSimdLevel lvl);

/* -------------------------------------------------------------------------
 * GEMM: C[M,N] = alpha * A[M,K] @ B[K,N] + beta * C[M,N]
 * ------------------------------------------------------------------------- */

/* Generic blocked scalar GEMM (always available, used as fallback) */
void SNEPPX_gemm_scalar(const float* A, const float* B, float* C,
                         int M, int N, int K,
                         float alpha, float beta);

/* SSE4.2 GEMM (4-wide) */
void SNEPPX_gemm_sse42(const float* A, const float* B, float* C,
                        int M, int N, int K, float alpha, float beta);

/* AVX2 GEMM (8-wide FMA) */
void SNEPPX_gemm_avx2(const float* A, const float* B, float* C,
                      int M, int N, int K, float alpha, float beta);

/* AVX-512 GEMM (16-wide FMA) */
void SNEPPX_gemm_avx512(const float* A, const float* B, float* C,
                        int M, int N, int K, float alpha, float beta);

/* Dispatch to the best available kernel */
void SNEPPX_gemm(const float* A, const float* B, float* C,
                 int M, int N, int K, float alpha, float beta);

/* Batched GEMM: C[b] = A[b] @ B[b]; arrays of [M,K] and [K,N] */
void SNEPPX_gemm_batched(const float* A, const float* B, float* C,
                         int batch, int M, int N, int K,
                         float alpha, float beta);

/* Strided batched GEMM (for attention heads) */
void SNEPPX_gemm_batched_strided(const float* A, const float* B, float* C,
                                 int batch, int M, int N, int K,
                                 int stride_a, int stride_b, int stride_c,
                                 float alpha, float beta);

/* -------------------------------------------------------------------------
 * Elementwise kernels (C = op(A, B) or C = f(A))
 * ------------------------------------------------------------------------- */

void SNEPPX_elem_add_avx2(const float* A, const float* B, float* C, size_t n);
void SNEPPX_elem_mul_avx2(const float* A, const float* B, float* C, size_t n);
void SNEPPX_elem_relu_avx2(const float* A, float* C, size_t n);
void SNEPPX_elem_gelu_avx2(const float* A, float* C, size_t n);
void SNEPPX_elem_silu_avx2(const float* A, float* C, size_t n);
void SNEPPX_elem_tanh_avx2(const float* A, float* C, size_t n);

/* Fused bias + activation: C = act(A @ W + bias) */
void SNEPPX_fused_linear_bias_avx2(const float* A, const float* W,
                                   const float* bias, float* C,
                                   int M, int N, int K,
                                   int act /*0=none,1=relu,2=gelu,3=silu*/);

/* -------------------------------------------------------------------------
 * Reductions
 * ------------------------------------------------------------------------- */

float SNEPPX_reduce_sum_avx2(const float* A, size_t n);
float SNEPPX_reduce_max_avx2(const float* A, size_t n);
void  SNEPPX_reduce_rowmax_avx2(const float* A, float* out, int rows, int cols);
void  SNEPPX_reduce_rowsum_exp_avx2(const float* A, float* out,
                                    int rows, int cols);

#ifdef __cplusplus
}
#endif

#endif /* SNEPPX_SIMD_GEMM_H */
