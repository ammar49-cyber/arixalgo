#include "onnx_format.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_onnx_load(const char* path) { (void)path; return NULL; }
void SNEPPX_onnx_destroy(void* model) { free(model); }
int SNEPPX_onnx_get_input_count(void* model) { (void)model; return 0; }
int SNEPPX_onnx_get_input_info(void* model, int idx, char* name, size_t name_max, size_t** shape, size_t* ndim, int* dtype) { (void)model; (void)idx; (void)name; (void)name_max; (void)shape; (void)ndim; (void)dtype; return 0; }
int SNEPPX_onnx_get_output_count(void* model) { (void)model; return 0; }
int SNEPPX_onnx_run(void* model, void** inputs, size_t num_inputs, void** outputs, size_t num_outputs) { (void)model; (void)inputs; (void)num_inputs; (void)outputs; (void)num_outputs; return 0; }
int SNEPPX_onnx_export(const char* path, void* graph, const char** input_names, size_t num_inputs, const char** output_names, size_t num_outputs) { (void)path; (void)graph; (void)input_names; (void)num_inputs; (void)output_names; (void)num_outputs; return 0; }
int SNEPPX_onnx_check(const char* path, char* error_msg, size_t error_max) { (void)path; (void)error_msg; (void)error_max; return 0; }
