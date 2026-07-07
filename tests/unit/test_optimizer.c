#include "gradient_optimization_suite.h"
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

static void test_optimizer_config_default(void) {
    ArixOptimizerConfig cfg = arix_optimizer_config_default(ARIX_ADAM);
    ASSERT(cfg.type == ARIX_ADAM, "adam default type");
    ASSERT(cfg.learning_rate > 0, "learning rate > 0");
}

static void test_sgd_create_step(void) {
    ArixOptimizerConfig cfg = arix_optimizer_config_default(ARIX_SGD);
    ArixOptimizer* opt = arix_optimizer_create(&cfg);
    ASSERT(opt != NULL, "sgd optimizer created");

    size_t shape[] = {4, 4};
    ArixTensor* params = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixTensor* grads = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* gd = (float*)grads->data;
    for (size_t i = 0; i < grads->size; i++) gd[i] = 0.1f;

    arix_optimizer_step(opt, &params, &grads, 1);
    float* pd = (float*)params->data;
    for (size_t i = 0; i < params->size; i++) {
        ASSERT(pd[i] < 1.0f, "parameter decreased after SGD step");
    }

    arix_tensor_destroy(grads);
    arix_tensor_destroy(params);
    arix_optimizer_destroy(opt);
}

static void test_adam_create_step(void) {
    ArixOptimizerConfig cfg = arix_optimizer_config_default(ARIX_ADAM);
    ArixOptimizer* opt = arix_optimizer_create(&cfg);
    ASSERT(opt != NULL, "adam optimizer created");

    size_t shape[] = {2, 2};
    ArixTensor* params = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixTensor* grads = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* gd = (float*)grads->data;
    for (size_t i = 0; i < grads->size; i++) gd[i] = 0.05f;

    arix_optimizer_step(opt, &params, &grads, 1);
    float* pd = (float*)params->data;
    for (size_t i = 0; i < params->size; i++) {
        ASSERT(pd[i] <= 1.0f, "parameter updated after Adam step");
    }

    arix_tensor_destroy(grads);
    arix_tensor_destroy(params);
    arix_optimizer_destroy(opt);
}

static void test_adamw_create_step(void) {
    ArixOptimizerConfig cfg = arix_optimizer_config_default(ARIX_ADAMW);
    ArixOptimizer* opt = arix_optimizer_create(&cfg);
    ASSERT(opt != NULL, "adamw optimizer created");

    size_t shape[] = {3, 3};
    ArixTensor* params = arix_tensor_ones(shape, 2, ARIX_FLOAT32);
    ArixTensor* grads = arix_tensor_create(shape, 2, ARIX_FLOAT32);
    float* gd = (float*)grads->data;
    for (size_t i = 0; i < grads->size; i++) gd[i] = 0.1f;

    arix_optimizer_step(opt, &params, &grads, 1);

    arix_tensor_destroy(grads);
    arix_tensor_destroy(params);
    arix_optimizer_destroy(opt);
}

static void test_lr_scheduler_step(void) {
    ArixOptimizerConfig cfg = arix_optimizer_config_default(ARIX_SGD);
    ArixOptimizer* opt = arix_optimizer_create(&cfg);

    ArixLRScheduler* sched = arix_lr_scheduler_create(ARIX_STEP, &cfg.learning_rate, 0.1f, 10);
    ASSERT(sched != NULL, "lr scheduler created");

    arix_lr_scheduler_step(sched, 0.0f);

    arix_lr_scheduler_destroy(sched);
    arix_optimizer_destroy(opt);
}

int main(void) {
    run_test("optimizer_config_default", test_optimizer_config_default);
    run_test("sgd_create_step", test_sgd_create_step);
    run_test("adam_create_step", test_adam_create_step);
    run_test("adamw_create_step", test_adamw_create_step);
    run_test("lr_scheduler_step", test_lr_scheduler_step);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
