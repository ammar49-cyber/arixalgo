#ifndef SNEPPX_NPU_DRIVER_H
#define SNEPPX_NPU_DRIVER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int  SNEPPX_npu_register_driver(void);
int  SNEPPX_npu_get_device_count(int* count);
int  SNEPPX_npu_get_device_props(int dev_id, void* props);
void* SNEPPX_npu_create_context(int device_id);
void  SNEPPX_npu_destroy_context(void* ctx);
int   SNEPPX_npu_mem_alloc(void** dev_ptr, size_t bytes, void* ctx);
int   SNEPPX_npu_mem_free(void* dev_ptr, void* ctx);
int   SNEPPX_npu_mem_htod(void* dev_dst, const void* host_src, size_t bytes, void* ctx);
int   SNEPPX_npu_mem_dtoh(void* host_dst, const void* dev_src, size_t bytes, void* ctx);
int   SNEPPX_npu_execute(void* exec, void** inputs, size_t num_inputs, void** outputs, size_t num_outputs, void* ctx);
#ifdef __cplusplus
}
#endif
#endif
