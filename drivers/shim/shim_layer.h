#ifndef SNEPPX_SHIM_LAYER_H
#define SNEPPX_SHIM_LAYER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int SNEPPX_shim_init(const char* backend_type);
void SNEPPX_shim_cleanup(void);
int SNEPPX_shim_get_device_count(int* count);
int SNEPPX_shim_get_device_props(int dev_id, char* name, size_t name_max, int* dev_type, size_t* global_mem);
void* SNEPPX_shim_create_context(int device_id);
void SNEPPX_shim_destroy_context(void* ctx);
int SNEPPX_shim_mem_alloc(void** dev_ptr, size_t bytes, void* ctx);
int SNEPPX_shim_mem_free(void* dev_ptr, void* ctx);
int SNEPPX_shim_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes, void* ctx);
int SNEPPX_shim_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes, void* ctx);
int SNEPPX_shim_launch_kernel(const char* kernel_name, void* ctx, void** args, size_t num_args, size_t global_size, size_t local_size);
int SNEPPX_shim_synchronize(void* ctx);
#ifdef __cplusplus
}
#endif
#endif
