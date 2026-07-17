#ifndef SNEPPX_QUALCOMM_DRIVER_H
#define SNEPPX_QUALCOMM_DRIVER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int  SNEPPX_qualcomm_register(void);
int  SNEPPX_qualcomm_get_device_count(int* count);
int  SNEPPX_qualcomm_get_device_props(int dev_id, char* name, size_t name_max, unsigned long long* total_mem);
void* SNEPPX_qualcomm_create_context(const char* model_path);
void  SNEPPX_qualcomm_destroy_context(void* ctx);
int   SNEPPX_qualcomm_set_input(void* ctx, const char* name, const float* data, size_t size);
int   SNEPPX_qualcomm_run_inference(void* ctx);
int   SNEPPX_qualcomm_get_output(void* ctx, const char* name, float* data, size_t size);
#ifdef __cplusplus
}
#endif
#endif
