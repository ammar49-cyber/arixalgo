#ifndef SNEPPX_ONNX_FORMAT_H
#define SNEPPX_ONNX_FORMAT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_onnx_load(const char* path);
void SNEPPX_onnx_destroy(void* model);
int SNEPPX_onnx_get_input_count(void* model);
int SNEPPX_onnx_get_input_info(void* model, int idx, char* name, size_t name_max, size_t** shape, size_t* ndim, int* dtype);
int SNEPPX_onnx_get_output_count(void* model);
int SNEPPX_onnx_run(void* model, void** inputs, size_t num_inputs, void** outputs, size_t num_outputs);
int SNEPPX_onnx_export(const char* path, void* graph, const char** input_names, size_t num_inputs, const char** output_names, size_t num_outputs);
int SNEPPX_onnx_check(const char* path, char* error_msg, size_t error_max);
#ifdef __cplusplus
}
#endif
#endif
