#include "safetensors.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_safetensors_open(const char* path, const char* mode) { (void)path; (void)mode; return NULL; }
void SNEPPX_safetensors_close(void* st) { free(st); }
int SNEPPX_safetensors_get_tensor_count(void* st) { (void)st; return 0; }
int SNEPPX_safetensors_get_tensor_names(void* st, char*** names, size_t* count) { (void)st; (void)names; (void)count; return 0; }
void* SNEPPX_safetensors_read_tensor(void* st, const char* name, size_t* ndim, size_t** shape, int* dtype) { (void)st; (void)name; (void)ndim; (void)shape; (void)dtype; return NULL; }
int SNEPPX_safetensors_write_tensor(void* st, const char* name, const void* data, const size_t* shape, size_t ndim, int dtype) { (void)st; (void)name; (void)data; (void)shape; (void)ndim; (void)dtype; return 0; }
int SNEPPX_safetensors_save(void* st) { (void)st; return 0; }
unsigned long long SNEPPX_safetensors_get_metadata(void* st, const char* key, char* value, size_t value_max) { (void)st; (void)key; (void)value; (void)value_max; return 0; }
