#include "oneapi_driver.h"
#include <stdlib.h>
#include <string.h>

int SNEPPX_oneapi_register(void) { return 0; }
int SNEPPX_oneapi_get_device_count(int* count) { if (count) *count = 0; return 0; }
int SNEPPX_oneapi_get_device_props(int dev_id, char* name, size_t name_max, size_t* global_mem) { (void)dev_id; if (name) snprintf(name, name_max, "Intel SYCL Device %d", dev_id); if (global_mem) *global_mem = 8ULL*1024*1024*1024; return 0; }
void* SNEPPX_oneapi_create_queue(int dev_id) { (void)dev_id; return calloc(1, 8); }
void SNEPPX_oneapi_destroy_queue(void* queue) { free(queue); }
int SNEPPX_oneapi_mem_alloc(void** dev_ptr, size_t bytes, void* queue) { (void)dev_ptr; (void)bytes; (void)queue; return 0; }
int SNEPPX_oneapi_mem_free(void* dev_ptr, void* queue) { (void)dev_ptr; (void)queue; return 0; }
int SNEPPX_oneapi_memcpy_htod(void* dev_dst, const void* host_src, size_t bytes, void* queue) { (void)dev_dst; (void)host_src; (void)bytes; (void)queue; return 0; }
int SNEPPX_oneapi_memcpy_dtoh(void* host_dst, const void* dev_src, size_t bytes, void* queue) { (void)host_dst; (void)dev_src; (void)bytes; (void)queue; return 0; }
int SNEPPX_oneapi_launch_kernel(const char* kernel_name, void* queue, void* args, size_t args_size, size_t global_size, size_t local_size) { (void)kernel_name; (void)queue; (void)args; (void)args_size; (void)global_size; (void)local_size; return 0; }
