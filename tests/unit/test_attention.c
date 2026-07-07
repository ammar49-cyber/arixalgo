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

static void test_tensor_eye(void) {
    ArixTensor* t = arix_tensor_eye(4, ARIX_FLOAT32);
    ASSERT(t != NULL, "eye created");
    ASSERT(t->shape[0] == 4, "eye rows");
    ASSERT(t->shape[1] == 4, "eye cols");
    float* d = (float*)t->data;
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < 4; j++)
            ASSERT_NEAR(d[i * 4 + j], (i == j) ? 1.0f : 0.0f, 1e-6f, "eye values");
    arix_tensor_destroy(t);
}

static void test_tensor_arange(void) {
    ArixTensor* t = arix_tensor_arange(0.0f, 5.0f, 1.0f, ARIX_FLOAT32);
    ASSERT(t != NULL, "arange created");
    float* d = (float*)t->data;
    for (size_t i = 0; i < 5; i++) ASSERT_NEAR(d[i], (float)i, 1e-6f, "arange values");
    arix_tensor_destroy(t);
}

static void test_tensor_reshape(void) {
    size_t shape[] = {2, 6};
    ArixTensor* t = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* d = (float*)t->data;
    for (size_t i = 0; i < 12; i++) d[i] = (float)i;
    size_t new_shape[] = {3, 4};
    ArixTensor* r = arix_tensor_reshape(t, new_shape, 2);
    ASSERT(r != NULL, "reshape created");
    ASSERT(r->shape[0] == 3, "reshape rows");
    ASSERT(r->shape[1] == 4, "reshape cols");
    arix_tensor_destroy(r);
    arix_tensor_destroy(t);
}

static void test_tensor_transpose(void) {
    size_t shape[] = {2, 3};
    ArixTensor* t = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* d = (float*)t->data;
    for (size_t i = 0; i < 6; i++) d[i] = (float)i;
    ArixTensor* tp = arix_tensor_transpose(t, 0, 1);
    ASSERT(tp != NULL, "transpose created");
    ASSERT(tp->shape[0] == 3, "transpose rows");
    ASSERT(tp->shape[1] == 2, "transpose cols");
    arix_tensor_destroy(tp);
    arix_tensor_destroy(t);
}

static void test_tensor_sum(void) {
    size_t shape[] = {2, 2};
    ArixTensor* t = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixTensor* s = arix_tensor_sum(t, 1);
    ASSERT(s != NULL, "sum created");
    float* sd = (float*)s->data;
    ASSERT_NEAR(sd[0], 2.0f, 1e-6f, "sum row 0");
    ASSERT_NEAR(sd[1], 2.0f, 1e-6f, "sum row 1");
    arix_tensor_destroy(s);
    arix_tensor_destroy(t);
}

static void test_tensor_concat(void) {
    size_t shape[] = {2, 3};
    ArixTensor* a = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixTensor* b = arix_tensor_zeros(shape, 2, ARIX_FLOAT32);
    ArixTensor* tensors[2] = {a, b};
    ArixTensor* c = arix_tensor_concat(tensors, 2, 0);
    ASSERT(c != NULL, "concat created");
    ASSERT(c->shape[0] == 4, "concat rows");
    ASSERT(c->shape[1] == 3, "concat cols");
    arix_tensor_destroy(c);
    arix_tensor_destroy(b);
    arix_tensor_destroy(a);
}

int main(void) {
    run_test("tensor_eye", test_tensor_eye);
    run_test("tensor_arange", test_tensor_arange);
    run_test("tensor_reshape", test_tensor_reshape);
    run_test("tensor_transpose", test_tensor_transpose);
    run_test("tensor_sum", test_tensor_sum);
    run_test("tensor_concat", test_tensor_concat);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
