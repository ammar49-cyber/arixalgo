#include "multidimensional_tensor_engine.h"

SNEPPXTensor* SNEPPX_tensor_create(const size_t* shape, size_t ndim, SNEPPXDtype dtype) {
    (void)shape; (void)ndim; (void)dtype;
    return NULL;
}

void SNEPPX_tensor_destroy(SNEPPXTensor* tensor) {
    (void)tensor;
}

float SNEPPX_tensor_get_f32(const SNEPPXTensor* tensor, const size_t* indices) {
    (void)tensor; (void)indices;
    return 0.0f;
}

size_t SNEPPX_tensor_dtype_size(SNEPPXDtype dtype) { (void)dtype; return 4; }
size_t SNEPPX_tensor_numel(const SNEPPXTensor* tensor) { (void)tensor; return 0; }
int SNEPPX_tensor_is_contiguous(const SNEPPXTensor* tensor) { (void)tensor; return 1; }
const char* SNEPPX_tensor_dtype_name(SNEPPXDtype dtype) { (void)dtype; return "UNKNOWN"; }

SNEPPXTensor* SNEPPX_tensor_empty(const size_t* shape, size_t ndim, SNEPPXDtype dtype) {
    (void)shape; (void)ndim; (void)dtype;
    return NULL;
}
SNEPPXTensor* SNEPPX_tensor_zeros(const size_t* shape, size_t ndim, SNEPPXDtype dtype) {
    (void)shape; (void)ndim; (void)dtype;
    return NULL;
}
SNEPPXTensor* SNEPPX_tensor_ones(const size_t* shape, size_t ndim, SNEPPXDtype dtype) {
    (void)shape; (void)ndim; (void)dtype;
    return NULL;
}
SNEPPXTensor* SNEPPX_tensor_randn(const size_t* shape, size_t ndim, SNEPPXDtype dtype) {
    (void)shape; (void)ndim; (void)dtype;
    return NULL;
}
SNEPPXTensor* SNEPPX_tensor_copy(const SNEPPXTensor* src) { (void)src; return NULL; }
SNEPPXTensor* SNEPPX_tensor_add(const SNEPPXTensor* a, const SNEPPXTensor* b) { (void)a; (void)b; return NULL; }
SNEPPXTensor* SNEPPX_tensor_matmul(const SNEPPXTensor* a, const SNEPPXTensor* b) { (void)a; (void)b; return NULL; }
SNEPPXTensor* SNEPPX_tensor_relu(const SNEPPXTensor* src) { (void)src; return NULL; }
void SNEPPX_tensor_print(const SNEPPXTensor* tensor) { (void)tensor; }
