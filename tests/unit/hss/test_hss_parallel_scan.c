#include "hierarchical_state_space.h"
#include "polymorphic_memory_allocator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

static void test_parallel_vs_sequential(void) {
    ArixHSSConfig cfg = arix_hss_config_default();
    cfg.state_dim = 8;
    cfg.input_dim = 16;
    cfg.output_dim = 16;
    cfg.num_layers = 1;
    cfg.seq_len = 32;
    cfg.use_hierarchical = 0;
    cfg.use_parallel_scan = 1;

    ArixHSSModel* model = arix_hss_model_create(&cfg, 42);
    ASSERT(model != NULL, "model created");

    size_t shape_in[] = {1, cfg.seq_len, cfg.input_dim};
    ArixTensor* input = arix_tensor_create(shape_in, 3, ARIX_FLOAT32);
    ASSERT(input != NULL, "input tensor created");

    float* data = (float*)input->data;
    for (size_t i = 0; i < input->size; i++) data[i] = ((float)(i % 7) - 3.0f) * 0.5f;

    ArixTensor* output = NULL;
    int ret = arix_hss_forward(model, input, &output);
    ASSERT(ret == 0, "forward pass with parallel scan succeeded");
    ASSERT(output != NULL, "output tensor created");
    ASSERT(output->size == cfg.seq_len * cfg.output_dim, "output size correct");

    arix_tensor_destroy(output);
    arix_tensor_destroy(input);
    arix_hss_model_destroy(model);
}

static void test_parallel_scan_disabled_fallback(void) {
    ArixHSSConfig cfg = arix_hss_config_default();
    cfg.state_dim = 8;
    cfg.input_dim = 16;
    cfg.output_dim = 16;
    cfg.num_layers = 1;
    cfg.seq_len = 32;
    cfg.use_hierarchical = 0;
    cfg.use_parallel_scan = 0;

    ArixHSSModel* model = arix_hss_model_create(&cfg, 42);
    ASSERT(model != NULL, "model created");

    size_t shape_in[] = {1, cfg.seq_len, cfg.input_dim};
    ArixTensor* input = arix_tensor_create(shape_in, 3, ARIX_FLOAT32);
    ASSERT(input != NULL, "input tensor created");

    float* data = (float*)input->data;
    for (size_t i = 0; i < input->size; i++) data[i] = ((float)(i % 7) - 3.0f) * 0.5f;

    ArixTensor* output = NULL;
    int ret = arix_hss_forward(model, input, &output);
    ASSERT(ret == 0, "forward pass with sequential scan succeeded");
    ASSERT(output != NULL, "output tensor created");

    arix_tensor_destroy(output);
    arix_tensor_destroy(input);
    arix_hss_model_destroy(model);
}

static void test_parallel_scan_matches_sequential(void) {
    ArixHSSConfig cfg = arix_hss_config_default();
    cfg.state_dim = 4;
    cfg.input_dim = 8;
    cfg.output_dim = 8;
    cfg.num_layers = 2;
    cfg.seq_len = 16;
    cfg.use_hierarchical = 0;

    ArixHSSModel* model_seq = arix_hss_model_create(&cfg, 100);
    ASSERT(model_seq != NULL, "seq model created");
    ArixHSSModel* model_par = arix_hss_model_create(&cfg, 100);
    ASSERT(model_par != NULL, "par model created");

    model_par->config.use_parallel_scan = 1;

    size_t shape_in[] = {1, cfg.seq_len, cfg.input_dim};
    ArixTensor* input = arix_tensor_create(shape_in, 3, ARIX_FLOAT32);
    ASSERT(input != NULL, "input tensor created");

    float* data = (float*)input->data;
    for (size_t i = 0; i < input->size; i++) data[i] = sinf((float)i * 0.1f);

    ArixTensor* out_seq = NULL;
    arix_hss_forward(model_seq, input, &out_seq);
    ASSERT(out_seq != NULL, "seq output");

    ArixTensor* out_par = NULL;
    arix_hss_forward(model_par, input, &out_par);
    ASSERT(out_par != NULL, "par output");

    float* seq_data = (float*)out_seq->data;
    float* par_data = (float*)out_par->data;
    for (size_t i = 0; i < out_seq->size; i++) {
        ASSERT_NEAR(seq_data[i], par_data[i], 1e-3f, "parallel scan matches sequential");
    }

    arix_tensor_destroy(out_seq);
    arix_tensor_destroy(out_par);
    arix_tensor_destroy(input);
    arix_hss_model_destroy(model_seq);
    arix_hss_model_destroy(model_par);
}

int main(void) {
    run_test("parallel_vs_sequential", test_parallel_vs_sequential);
    run_test("parallel_scan_disabled_fallback", test_parallel_scan_disabled_fallback);
    run_test("parallel_scan_matches_sequential", test_parallel_scan_matches_sequential);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
