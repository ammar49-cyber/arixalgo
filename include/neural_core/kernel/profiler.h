#ifndef SNEPPX_PROFILER_H
#define SNEPPX_PROFILER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of named profiler entries */
#define SNEPPX_PROFILER_MAX_ENTRIES 256
#define SNEPPX_PROFILER_NAME_MAX 64

/* Profiler entry: aggregates timing for a named operation */
typedef struct {
    char name[SNEPPX_PROFILER_NAME_MAX];
    int num_calls;
    float total_time_ms;
    float min_time_ms;
    float max_time_ms;
    float avg_time_ms;
} SNEPPX_ProfilerEntry;

/* Global profiler state */
typedef struct {
    SNEPPX_ProfilerEntry entries[SNEPPX_PROFILER_MAX_ENTRIES];
    int num_entries;
    int enabled;
} SNEPPX_Profiler;

/* Initialize/finalize */
int  SNEPPX_profiler_init(SNEPPX_Profiler* prof);
void SNEPPX_profiler_destroy(SNEPPX_Profiler* prof);
void SNEPPX_profiler_enable(SNEPPX_Profiler* prof, int enabled);

/* Record a timing sample for a named operation */
int  SNEPPX_profiler_record(SNEPPX_Profiler* prof, const char* name, float elapsed_ms);

/* Get entry by name */
SNEPPX_ProfilerEntry* SNEPPX_profiler_get(SNEPPX_Profiler* prof, const char* name);

/* Reset all entries */
void SNEPPX_profiler_reset(SNEPPX_Profiler* prof);

/* Print summary to stdout */
void SNEPPX_profiler_print(const SNEPPX_Profiler* prof);

/* Export to JSON string (caller must free) */
char* SNEPPX_profiler_to_json(const SNEPPX_Profiler* prof);

/* CUDA kernel duration helper: wraps a kernel launch with timing */
typedef struct {
    cudaEvent_t start;
    cudaEvent_t end;
} SNEPPX_KernelTimer;

int  SNEPPX_kernel_timer_init(SNEPPX_KernelTimer* kt);
void SNEPPX_kernel_timer_start(SNEPPX_KernelTimer* kt, cudaStream_t stream);
float SNEPPX_kernel_timer_stop(SNEPPX_KernelTimer* kt, cudaStream_t stream);
void SNEPPX_kernel_timer_destroy(SNEPPX_KernelTimer* kt);

/* Convenience macro: profiles a kernel launch */
#define SNEPPX_PROFILE_KERNEL(prof, name, stream, kernel_call) \
    do { \
        cudaEvent_t _p_start, _p_end; \
        cudaEventCreate(&_p_start); \
        cudaEventCreate(&_p_end); \
        cudaEventRecord(_p_start, stream); \
        kernel_call; \
        cudaEventRecord(_p_end, stream); \
        cudaEventSynchronize(_p_end); \
        float _p_ms = 0; \
        cudaEventElapsedTime(&_p_ms, _p_start, _p_end); \
        SNEPPX_profiler_record(prof, name, _p_ms); \
        cudaEventDestroy(_p_start); \
        cudaEventDestroy(_p_end); \
    } while(0)

/* Stack-based NVTX-style range markers (no NVTX dependency) */
void SNEPPX_range_push(const char* name);
void SNEPPX_range_pop(void);
int  SNEPPX_range_get_depth(void);

#ifdef __cplusplus
}
#endif

#endif /* SNEPPX_PROFILER_H */
