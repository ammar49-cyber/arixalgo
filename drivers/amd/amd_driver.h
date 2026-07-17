#ifndef SNEPPX_AMD_DRIVER_H
#define SNEPPX_AMD_DRIVER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int  SNEPPX_amd_register(void);
int  SNEPPX_amd_get_device_count(int* count);
int  SNEPPX_amd_get_device_props(int dev_id, char* name, size_t name_max, size_t* global_mem, int* compute_units);
void* SNEPPX_amd_create_context(int device_id);
void  SNEPPX_amd_destroy_context(void* ctx);
int   SNEPPX_amd_mem_alloc(void** dev_ptr, size_t bytes);
int   SNEPPX_amd_mem_free(void* dev_ptr);
int   SNEPPX_amd_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes);
int   SNEPPX_amd_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes);
int   SNEPPX_amd_launch_kernel(const char* kernel_name, void* queue, void** args, size_t num_args, size_t global_size, size_t local_size);
#ifdef __cplusplus
}
#endif
#endif
