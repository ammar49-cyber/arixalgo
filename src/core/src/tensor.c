#include "arix_tensor.h"
#include "arix_memory.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

static size_t compute_offset(const ArixTensor* tensor, const size_t* indices) {
    size_t offset = 0;
    for (size_t i = 0; i < tensor->ndim; i++) {
        offset += indices[i] * tensor->strides[i];
    }
    return offset;
}

ArixTensor* arix_tensor_create(const size_t* shape, size_t ndim, ArixDtype dtype) {
    ArixTensor* tensor = (ArixTensor*)arix_malloc(sizeof(ArixTensor), 64);
    if (!tensor) return NULL;

    tensor->ndim = ndim;
    tensor->dtype = dtype;
    tensor->item_size = arix_tensor_dtype_size(dtype);

    tensor->shape = (size_t*)arix_malloc(ndim * sizeof(size_t), 64);
    tensor->strides = (size_t*)arix_malloc(ndim * sizeof(size_t), 64);
    if (!tensor->shape || !tensor->strides) {
        arix_free(tensor->shape, ndim * sizeof(size_t));
        arix_free(tensor->strides, ndim * sizeof(size_t));
        arix_free(tensor, sizeof(ArixTensor));
        return NULL;
    }

    size_t total = 1;
    for (size_t i = 0; i < ndim; i++) {
        tensor->shape[i] = shape[i];
        total *= shape[i];
    }
    tensor->size = total;

    size_t stride = 1;
    for (size_t i = ndim; i > 0; i--) {
        tensor->strides[i - 1] = stride;
        stride *= shape[i - 1];
    }

    tensor->data = arix_malloc(total * tensor->item_size, 64);
    if (!tensor->data) {
        arix_free(tensor->shape, ndim * sizeof(size_t));
        arix_free(tensor->strides, ndim * sizeof(size_t));
        arix_free(tensor, sizeof(ArixTensor));
        return NULL;
    }

    return tensor;
}

void arix_tensor_destroy(ArixTensor* tensor) {
    if (!tensor) return;
    arix_free(tensor->data, tensor->size * tensor->item_size);
    arix_free(tensor->shape, tensor->ndim * sizeof(size_t));
    arix_free(tensor->strides, tensor->ndim * sizeof(size_t));
    arix_free(tensor, sizeof(ArixTensor));
}

float arix_tensor_get_f32(const ArixTensor* tensor, const size_t* indices) {
    if (tensor->dtype != ARIX_FLOAT32) return 0.0f;
    size_t offset = compute_offset(tensor, indices);
    return ((float*)tensor->data)[offset];
}

void arix_tensor_set_f32(ArixTensor* tensor, const size_t* indices, float value) {
    if (tensor->dtype != ARIX_FLOAT32) return;
    size_t offset = compute_offset(tensor, indices);
    ((float*)tensor->data)[offset] = value;
}

ArixTensor* arix_tensor_zeros(const size_t* shape, size_t ndim, ArixDtype dtype) {
    ArixTensor* tensor = arix_tensor_create(shape, ndim, dtype);
    if (!tensor) return NULL;
    memset(tensor->data, 0, tensor->size * tensor->item_size);
    return tensor;
}

ArixTensor* arix_tensor_ones(const size_t* shape, size_t ndim, ArixDtype dtype) {
    ArixTensor* tensor = arix_tensor_create(shape, ndim, dtype);
    if (!tensor) return NULL;
    float* data = (float*)tensor->data;
    for (size_t i = 0; i < tensor->size; i++) {
        data[i] = 1.0f;
    }
    return tensor;
}

static unsigned long lcg_state = 123456789;

static float uniform_01(void) {
    lcg_state = lcg_state * 1103515245UL + 12345UL;
    return (float)((lcg_state >> 16) & 0x7FFF) / 32767.0f;
}

ArixTensor* arix_tensor_randn(const size_t* shape, size_t ndim, ArixDtype dtype) {
    ArixTensor* tensor = arix_tensor_create(shape, ndim, dtype);
    if (!tensor) return NULL;
    float* data = (float*)tensor->data;
    for (size_t i = 0; i < tensor->size; i += 2) {
        float u1 = uniform_01();
        float u2 = uniform_01();
        float r = sqrtf(-2.0f * logf(u1 + 1e-10f));
        float theta = 2.0f * 3.14159265f * u2;
        data[i] = r * cosf(theta);
        if (i + 1 < tensor->size) {
            data[i + 1] = r * sinf(theta);
        }
    }
    return tensor;
}

void arix_tensor_print(const ArixTensor* tensor) {
    if (!tensor) return;
    printf("Tensor shape: [");
    for (size_t i = 0; i < tensor->ndim; i++) {
        printf("%zu", tensor->shape[i]);
        if (i < tensor->ndim - 1) printf(", ");
    }
    printf("]\n");
    printf("Dtype: ");
    switch (tensor->dtype) {
        case ARIX_FLOAT32: printf("float32"); break;
        case ARIX_FLOAT64: printf("float64"); break;
        case ARIX_INT32:   printf("int32"); break;
        case ARIX_INT64:   printf("int64"); break;
        case ARIX_BOOL:    printf("bool"); break;
    }
    printf("\n");
    printf("Size: %zu elements\n", tensor->size);
    if (tensor->dtype == ARIX_FLOAT32) {
        size_t n = tensor->size < 20 ? tensor->size : 20;
        printf("Data (first %zu): [", n);
        float* data = (float*)tensor->data;
        for (size_t i = 0; i < n; i++) {
            printf("%f", data[i]);
            if (i < n - 1) printf(", ");
        }
        printf("]\n");
    }
}

size_t arix_tensor_dtype_size(ArixDtype dtype) {
    switch (dtype) {
        case ARIX_FLOAT32: return sizeof(float);
        case ARIX_FLOAT64: return sizeof(double);
        case ARIX_INT32:   return sizeof(int32_t);
        case ARIX_INT64:   return sizeof(int64_t);
        case ARIX_BOOL:    return sizeof(uint8_t);
    }
    return 0;
}
