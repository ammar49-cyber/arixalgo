#include "rocm_driver.h"
#include <stdio.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("FAIL: %s (%s)\n", msg, #cond); \
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

static void test_rocm_init_shutdown(void) {
    int ret = arix_rocm_init();
    ASSERT(ret == 0, "rocm_init returns 0 (stub)");
    arix_rocm_shutdown();
}

static void test_rocm_device_count(void) {
    int count = arix_rocm_device_count();
    ASSERT(count >= 0, "device count >= 0");
}

static void test_rocm_malloc_free(void) {
    void* ptr = NULL;
    int ret = arix_rocm_malloc(&ptr, 1024);
    ASSERT(ret == 0, "rocm_malloc returns 0 (stub)");
    if (ptr) arix_rocm_free(ptr);
}

static void test_rocm_memcpy(void) {
    float host[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    void* device = NULL;
    arix_rocm_malloc(&device, 16);
    int ret = arix_rocm_memcpy_to_device(device, host, 16);
    ASSERT(ret == 0, "memcpy to device (stub)");
    if (device) arix_rocm_free(device);
}

int main(void) {
    run_test("rocm_init_shutdown", test_rocm_init_shutdown);
    run_test("rocm_device_count", test_rocm_device_count);
    run_test("rocm_malloc_free", test_rocm_malloc_free);
    run_test("rocm_memcpy", test_rocm_memcpy);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
