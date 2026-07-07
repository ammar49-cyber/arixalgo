#include "multidimensional_tensor_engine.h"
#include <stdio.h>
#include <math.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s (%s)\n", msg, #cond); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NEAR(a, b, eps, msg) do { \
    if (fabsf((a) - (b)) > (eps)) { \
        printf("FAIL: %s (got %f, expected %f)\n", msg, (float)(a), (float)(b)); \
        tests_failed++; \
        return; \
    } \
} while(0)

static void run_test(const char* name, void (*test_fn)(void)) {
    printf("Running %s... ", name);
    fflush(stdout);
    test_fn();
    printf("PASS\n");
    tests_passed++;
}

static void test_tensor_relu(void) {
    size_t shape[] = {2, 3};
    ArixTensor* t = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* d = (float*)t->data;
    d[0] = -1.0f; d[1] = 0.0f; d[2] = 2.0f;
    d[3] = 3.0f; d[4] = -5.0f; d[5] = 0.5f;

    ArixTensor* out = arix_tensor_relu(t);
    ASSERT(out != NULL, "relu output created");

    float* od = (float*)out->data;
    ASSERT_NEAR(od[0], 0.0f, 1e-6f, "relu(-1) = 0");
    ASSERT_NEAR(od[1], 0.0f, 1e-6f, "relu(0) = 0");
    ASSERT_NEAR(od[2], 2.0f, 1e-6f, "relu(2) = 2");
    ASSERT_NEAR(od[3], 3.0f, 1e-6f, "relu(3) = 3");
    ASSERT_NEAR(od[4], 0.0f, 1e-6f, "relu(-5) = 0");
    ASSERT_NEAR(od[5], 0.5f, 1e-6f, "relu(0.5) = 0.5");

    arix_tensor_destroy(out);
    arix_tensor_destroy(t);
}

static void test_tensor_softmax(void) {
    size_t shape[] = {1, 4};
    ArixTensor* t = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* d = (float*)t->data;
    d[0] = 1.0f; d[1] = 2.0f; d[2] = 3.0f; d[3] = 4.0f;

    ArixTensor* out = arix_tensor_softmax(t, 1);
    ASSERT(out != NULL, "softmax output created");

    float* od = (float*)out->data;
    float sum = 0.0f;
    for (size_t i = 0; i < 4; i++) sum += od[i];
    ASSERT_NEAR(sum, 1.0f, 1e-4f, "softmax sums to 1");
    ASSERT(od[3] > od[2], "softmax preserves order");

    arix_tensor_destroy(out);
    arix_tensor_destroy(t);
}

static void test_tensor_layer_norm(void) {
    size_t shape[] = {1, 4};
    ArixTensor* t = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* d = (float*)t->data;
    d[0] = 1.0f; d[1] = 2.0f; d[2] = 3.0f; d[3] = 4.0f;

    ArixTensor* gamma = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixTensor* beta = arix_tensor_zeros(shape, 2, ARIX_FLOAT32);

    ArixTensor* out = arix_tensor_layer_norm(t, gamma, beta, 1e-5f);
    ASSERT(out != NULL, "layer norm output created");

    float* od = (float*)out->data;
    float mean = 0.0f;
    for (size_t i = 0; i < 4; i++) mean += od[i];
    mean /= 4.0f;
    ASSERT_NEAR(mean, 0.0f, 1e-4f, "layer norm output mean ~ 0");

    arix_tensor_destroy(out);
    arix_tensor_destroy(beta);
    arix_tensor_destroy(gamma);
    arix_tensor_destroy(t);
}

static void test_tensor_matmul(void) {
    size_t shape_a[] = {2, 3};
    size_t shape_b[] = {3, 2};
    ArixTensor* a = arix_tensor_create(shape_a, 2, ARIX_FLOAT32);
    ArixTensor* b = arix_tensor_create(shape_b, 2, ARIX_FLOAT32);
    float* ad = (float*)a->data;
    float* bd = (float*)b->data;
    for (size_t i = 0; i < 6; i++) { ad[i] = (float)(i + 1); bd[i] = (float)(i + 1) * 0.5f; }

    ArixTensor* c = arix_tensor_matmul(a, b);
    ASSERT(c != NULL, "matmul output created");
    ASSERT(c->shape[0] == 2, "matmul rows correct");
    ASSERT(c->shape[1] == 2, "matmul cols correct");

    arix_tensor_destroy(c);
    arix_tensor_destroy(b);
    arix_tensor_destroy(a);
}

static void test_tensor_dropout(void) {
    size_t shape[] = {2, 5};
    ArixTensor* t = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixTensor* out = arix_tensor_dropout(t, 0.5f, 42);
    ASSERT(out != NULL, "dropout output created");

    float* od = (float*)out->data;
    float sum = 0.0f;
    for (size_t i = 0; i < 10; i++) sum += od[i];
    ASSERT(sum > 0.0f, "dropout preserves some values");

    arix_tensor_destroy(out);
    arix_tensor_destroy(t);
}

int main(void) {
    run_test("tensor_relu", test_tensor_relu);
    run_test("tensor_softmax", test_tensor_softmax);
    run_test("tensor_layer_norm", test_tensor_layer_norm);
    run_test("tensor_matmul", test_tensor_matmul);
    run_test("tensor_dropout", test_tensor_dropout);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
