#ifndef SNEPPX_SAFETENSORS_H
#define SNEPPX_SAFETENSORS_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_safetensors_open(const char* path, const char* mode);
void SNEPPX_safetensors_close(void* st);
int SNEPPX_safetensors_get_tensor_count(void* st);
int SNEPPX_safetensors_get_tensor_names(void* st, char*** names, size_t* count);
void* SNEPPX_safetensors_read_tensor(void* st, const char* name, size_t* ndim, size_t** shape, int* dtype);
int SNEPPX_safetensors_write_tensor(void* st, const char* name, const void* data, const size_t* shape, size_t ndim, int dtype);
int SNEPPX_safetensors_save(void* st);
unsigned long long SNEPPX_safetensors_get_metadata(void* st, const char* key, char* value, size_t value_max);
#ifdef __cplusplus
}
#endif
#endif
