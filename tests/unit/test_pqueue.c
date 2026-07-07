#include "polymorphic_memory_allocator.h"
#include <stdio.h>
#include <string.h>

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

static void test_pqueue_create_destroy(void) {
    ArixPriorityQueue* pq = arix_pqueue_create(10);
    ASSERT(pq != NULL, "pqueue created");
    ASSERT(arix_pqueue_size(pq) == 0, "empty pqueue");
    arix_pqueue_destroy(pq);
}

static void test_pqueue_push_pop(void) {
    ArixPriorityQueue* pq = arix_pqueue_create(10);
    arix_pqueue_push(pq, 3.0f, (void*)3);
    arix_pqueue_push(pq, 1.0f, (void*)1);
    arix_pqueue_push(pq, 2.0f, (void*)2);
    ASSERT(arix_pqueue_size(pq) == 3, "three items");

    void* val = arix_pqueue_pop(pq);
    ASSERT(val == (void*)1, "pop lowest priority");
    val = arix_pqueue_pop(pq);
    ASSERT(val == (void*)2, "pop second lowest");
    val = arix_pqueue_pop(pq);
    ASSERT(val == (void*)3, "pop highest priority");
    ASSERT(arix_pqueue_size(pq) == 0, "empty after pops");
    arix_pqueue_destroy(pq);
}

static void test_pqueue_peek(void) {
    ArixPriorityQueue* pq = arix_pqueue_create(10);
    arix_pqueue_push(pq, 5.0f, (void*)42);
    void* val = arix_pqueue_peek(pq);
    ASSERT(val == (void*)42, "peek returns top");
    ASSERT(arix_pqueue_size(pq) == 1, "size unchanged after peek");
    arix_pqueue_destroy(pq);
}

int main(void) {
    run_test("pqueue_create_destroy", test_pqueue_create_destroy);
    run_test("pqueue_push_pop", test_pqueue_push_pop);
    run_test("pqueue_peek", test_pqueue_peek);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
