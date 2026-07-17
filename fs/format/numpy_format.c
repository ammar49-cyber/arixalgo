#include "numpy_format.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_npy_load(const char* path) { (void)path; return NULL; }
void SNEPPX_npy_destroy(void* arr) { free(arr); }
void* SNEPPX_npy_get_data(void* arr) { (void)arr; return NULL; }
size_t SNEPPX_npy_get_size(void* arr) { (void)arr; return 0; }
int SNEPPX_npy_get_ndim(void* arr) { (void)arr; return 0; }
const size_t* SNEPPX_npy_get_shape(void* arr) { (void)arr; return NULL; }
int SNEPPX_npy_get_dtype(void* arr) { (void)arr; return 0; }
int SNEPPX_npy_save(const char* path, const void* data, const size_t* shape, size_t ndim, int dtype) { (void)path; (void)data; (void)shape; (void)ndim; (void)dtype; return 0; }
int SNEPPX_npz_load(const char* path, char*** keys, void*** arrays, size_t* count) { (void)path; (void)keys; (void)arrays; (void)count; return 0; }
int SNEPPX_npz_save(const char* path, const char** keys, const void** data_ptrs, const size_t** shapes, const size_t* ndims, const int* dtypes, size_t count) { (void)path; (void)keys; (void)data_ptrs; (void)shapes; (void)ndims; (void)dtypes; (void)count; return 0; }
void SNEPPX_npz_free(char** keys, void** arrays, size_t count) { (void)keys; (void)arrays; (void)count; }
