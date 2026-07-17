#ifndef SNEPPX_ONEAPI_DRIVER_H
#define SNEPPX_ONEAPI_DRIVER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int  SNEPPX_oneapi_register(void);
int  SNEPPX_oneapi_get_device_count(int* count);
int  SNEPPX_oneapi_get_device_props(int dev_id, char* name, size_t name_max, size_t* global_mem);
void* SNEPPX_oneapi_create_queue(int dev_id);
void  SNEPPX_oneapi_destroy_queue(void* queue);
int   SNEPPX_oneapi_mem_alloc(void** dev_ptr, size_t bytes, void* queue);
int   SNEPPX_oneapi_mem_free(void* dev_ptr, void* queue);
int   SNEPPX_oneapi_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes, void* queue);
int   SNEPPX_oneapi_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes, void* queue);
int   SNEPPX_oneapi_launch_kernel(const char* kernel_name, void* queue, void* args, size_t args_size, size_t global_size, size_t local_size);
#ifdef __cplusplus
}
#endif
#endif
