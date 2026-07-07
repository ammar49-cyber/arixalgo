#include "automatic_differentiation_framework.h"
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

static void test_relu_forward(void) {
    ArixTape* tape = arix_tape_new();
    size_t shape[] = {3};
    ArixTensor* data = arix_tensor_create(shape, 1, ARIX_FLOAT32);
    float* d = (float*)data->data;
    d[0] = -2.0f; d[1] = 0.0f; d[2] = 3.0f;
    ArixVariable* v = arix_variable_new(tape, data, ARIX_F32, ARIX_TRAINABLE);
    ArixVariable* out = arix_relu(tape, v);
    ASSERT(out != NULL, "relu output");
    float* od = (float*)out->data->data;
    ASSERT_NEAR(od[0], 0.0f, 1e-6f, "relu negative clamped");
    ASSERT_NEAR(od[1], 0.0f, 1e-6f, "relu zero");
    ASSERT_NEAR(od[2], 3.0f, 1e-6f, "relu positive unchanged");
    arix_tape_free(tape);
}

static void test_gelu_forward(void) {
    ArixTape* tape = arix_tape_new();
    size_t shape[] = {2};
    ArixTensor* data = arix_tensor_create(shape, 1, ARIX_FLOAT32);
    float* d = (float*)data->data;
    d[0] = 0.0f; d[1] = 1.0f;
    ArixVariable* v = arix_variable_new(tape, data, ARIX_F32, ARIX_TRAINABLE);
    ArixVariable* out = arix_gelu(tape, v);
    ASSERT(out != NULL, "gelu output");
    float* od = (float*)out->data->data;
    ASSERT_NEAR(od[0], 0.0f, 1e-2f, "gelu(0) ~ 0");
    ASSERT_NEAR(od[1], 0.841f, 1e-2f, "gelu(1) ~ 0.841");
    arix_tape_free(tape);
}

static void test_silu_forward(void) {
    ArixTape* tape = arix_tape_new();
    size_t shape[] = {2};
    ArixTensor* data = arix_tensor_create(shape, 1, ARIX_FLOAT32);
    float* d = (float*)data->data;
    d[0] = 0.0f; d[1] = 2.0f;
    ArixVariable* v = arix_variable_new(tape, data, ARIX_F32, ARIX_TRAINABLE);
    ArixVariable* out = arix_silu(tape, v);
    ASSERT(out != NULL, "silu output");
    float* od = (float*)out->data->data;
    ASSERT_NEAR(od[0], 0.0f, 1e-2f, "silu(0) ~ 0");
    ASSERT_NEAR(od[1], 1.761f, 1e-2f, "silu(2) ~ 1.761");
    arix_tape_free(tape);
}

int main(void) {
    run_test("relu_forward", test_relu_forward);
    run_test("gelu_forward", test_gelu_forward);
    run_test("silu_forward", test_silu_forward);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
