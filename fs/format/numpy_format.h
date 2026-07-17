#ifndef SNEPPX_NUMPY_FORMAT_H
#define SNEPPX_NUMPY_FORMAT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_npy_load(const char* path);
void SNEPPX_npy_destroy(void* arr);
void* SNEPPX_npy_get_data(void* arr);
size_t SNEPPX_npy_get_size(void* arr);
int SNEPPX_npy_get_ndim(void* arr);
const size_t* SNEPPX_npy_get_shape(void* arr);
int SNEPPX_npy_get_dtype(void* arr);
int SNEPPX_npy_save(const char* path, const void* data, const size_t* shape, size_t ndim, int dtype);
int SNEPPX_npz_load(const char* path, char*** keys, void*** arrays, size_t* count);
int SNEPPX_npz_save(const char* path, const char** keys, const void** data_ptrs, const size_t** shapes, const size_t* ndims, const int* dtypes, size_t count);
void SNEPPX_npz_free(char** keys, void** arrays, size_t count);
#ifdef __cplusplus
}
#endif
#endif
