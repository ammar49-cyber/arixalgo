#include "automatic_differentiation_framework.h"
#include "polymorphic_memory_allocator.h"
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

static void test_tape_create_destroy(void) {
    ArixTape* tape = arix_tape_create();
    ASSERT(tape != NULL, "tape created");
    arix_tape_destroy(tape);
}

static void test_variable_create(void) {
    size_t shape[] = {2, 3};
    ArixTensor* t = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    ASSERT(t != NULL, "tensor created");
    ArixVariable* v = arix_variable_create(t, 1);
    ASSERT(v != NULL, "variable created");
    ASSERT(v->requires_grad == 1, "requires_grad set");
    arix_variable_destroy(v);
}

static void test_tape_record_and_backward(void) {
    ArixTape* tape = arix_tape_create();
    size_t shape[] = {2, 2};
    ArixTensor* a_t = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixVariable* a = arix_variable_create(a_t, 1);
    arix_tape_record(tape, a);

    ArixTensor* b_t = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixVariable* b = arix_variable_create(b_t, 1);
    arix_tape_record(tape, b);

    ArixVariable* c = arix_add(tape, a, b);
    ASSERT(c != NULL, "add op created");

    ArixVariable* d = arix_mul(tape, c, a);
    ASSERT(d != NULL, "mul op created");

    arix_tape_backward(tape, d);
    ASSERT(a->grad != NULL, "gradient computed for a");
    ASSERT(b->grad != NULL, "gradient computed for b");

    arix_variable_destroy(d);
    arix_variable_destroy(c);
    arix_variable_destroy(b);
    arix_variable_destroy(a);
    arix_tape_destroy(tape);
}

static void test_tape_ops(void) {
    ArixTape* tape = arix_tape_create();
    size_t shape[] = {3, 1};
    ArixTensor* t = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixVariable* v = arix_variable_create(t, 1);
    arix_tape_record(tape, v);

    ArixVariable* r1 = arix_relu(tape, v);
    ASSERT(r1 != NULL, "relu op");
    ArixVariable* r2 = arix_sigmoid(tape, v);
    ASSERT(r2 != NULL, "sigmoid op");
    ArixVariable* r3 = arix_exp(tape, v);
    ASSERT(r3 != NULL, "exp op");
    ArixVariable* r4 = arix_log(tape, v);
    ASSERT(r4 != NULL, "log op");
    ArixVariable* r5 = arix_neg(tape, v);
    ASSERT(r5 != NULL, "neg op");

    arix_tape_backward(tape, r5);
    ASSERT(v->grad != NULL, "grad computed after sequence");

    arix_variable_destroy(r5);
    arix_variable_destroy(r4);
    arix_variable_destroy(r3);
    arix_variable_destroy(r2);
    arix_variable_destroy(r1);
    arix_variable_destroy(v);
    arix_tape_destroy(tape);
}

int main(void) {
    run_test("tape_create_destroy", test_tape_create_destroy);
    run_test("variable_create", test_variable_create);
    run_test("tape_record_and_backward", test_tape_record_and_backward);
    run_test("tape_ops", test_tape_ops);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
