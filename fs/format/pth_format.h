#ifndef SNEPPX_PTH_FORMAT_H
#define SNEPPX_PTH_FORMAT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_pth_load(const char* path, const char* device);
void SNEPPX_pth_destroy(void* state);
void* SNEPPX_pth_get_tensor(void* state, const char* key);
int SNEPPX_pth_get_keys(void* state, char*** keys, size_t* count);
int SNEPPX_pth_set_tensor(void* state, const char* key, const void* data, const size_t* shape, size_t ndim, int dtype);
int SNEPPX_pth_save(void* state, const char* path);
#ifdef __cplusplus
}
#endif
#endif
