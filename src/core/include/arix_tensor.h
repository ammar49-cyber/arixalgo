#ifndef ARIX_TENSOR_H
#define ARIX_TENSOR_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    ARIX_FLOAT32,
    ARIX_FLOAT64,
    ARIX_INT32,
    ARIX_INT64,
    ARIX_BOOL
} ArixDtype;

typedef struct {
    void* data;
    size_t* shape;
    size_t* strides;
    size_t ndim;
    size_t size;
    size_t item_size;
    ArixDtype dtype;
} ArixTensor;

ArixTensor* arix_tensor_create(const size_t* shape, size_t ndim, ArixDtype dtype);
void arix_tensor_destroy(ArixTensor* tensor);
float arix_tensor_get_f32(const ArixTensor* tensor, const size_t* indices);
void arix_tensor_set_f32(ArixTensor* tensor, const size_t* indices, float value);
ArixTensor* arix_tensor_zeros(const size_t* shape, size_t ndim, ArixDtype dtype);
ArixTensor* arix_tensor_ones(const size_t* shape, size_t ndim, ArixDtype dtype);
ArixTensor* arix_tensor_randn(const size_t* shape, size_t ndim, ArixDtype dtype);
void arix_tensor_print(const ArixTensor* tensor);
size_t arix_tensor_dtype_size(ArixDtype dtype);

#endif /* ARIX_TENSOR_H */
