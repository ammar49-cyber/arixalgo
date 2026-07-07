#include "neural_programming_engine.h"
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

static void run_test(const char* name, void (*test_fn)(void)) {
    printf("Running %s... ", name);
    fflush(stdout);
    test_fn();
    printf("PASS\n");
    tests_passed++;
}

static void test_constant_fold_identity(void) {
    ArixNPEProgram* prog = arix_npe_program_create(64);
    ASSERT(prog != NULL, "program created");

    ArixNPEInstruction load = {ARIX_LOAD, 0, -1, -1, 0, {1, 1}, {0, 0}};
    arix_npe_program_append(prog, load);

    ArixNPEInstruction halt = {ARIX_HALT, -1, -1, -1, 0, {0, 0}, {0, 0}};
    arix_npe_program_append(prog, halt);

    ArixTensor mem;
    float mem_data[] = {42.0f};
    mem.data = mem_data;
    mem.size = 1;

    ArixNPEProgram* folded = arix_npe_jit_constant_fold(prog, &mem);
    ASSERT(folded != NULL, "folded program created");
    ASSERT(folded->num_instructions == 2, "same number of instructions");

    arix_npe_program_destroy(folded);
    arix_npe_program_destroy(prog);
}

static void test_constant_fold_add(void) {
    ArixNPEProgram* prog = arix_npe_program_create(64);
    ASSERT(prog != NULL, "program created");

    ArixNPEInstruction load_a = {ARIX_LOAD, 0, -1, -1, 0, {1, 1}, {0, 0}};
    arix_npe_program_append(prog, load_a);
    ArixNPEInstruction load_b = {ARIX_LOAD, 1, -1, -1, 1, {1, 1}, {0, 0}};
    arix_npe_program_append(prog, load_b);
    ArixNPEInstruction add = {ARIX_ADD, 2, 0, 1, 0, {0, 0}, {0, 0}};
    arix_npe_program_append(prog, add);
    ArixNPEInstruction halt = {ARIX_HALT, -1, -1, -1, 0, {0, 0}, {0, 0}};
    arix_npe_program_append(prog, halt);

    float mem_data[] = {10.0f, 20.0f};
    ArixTensor mem;
    mem.data = mem_data;
    mem.size = 2;

    ArixNPEProgram* folded = arix_npe_jit_constant_fold(prog, &mem);
    ASSERT(folded != NULL, "folded program created");
    ASSERT(folded->num_instructions == 4, "instructions preserved");

    arix_npe_program_destroy(folded);
    arix_npe_program_destroy(prog);
}

static void test_constant_fold_null_memory(void) {
    ArixNPEProgram* prog = arix_npe_program_create(16);
    ArixNPEInstruction load = {ARIX_LOAD, 0, -1, -1, 0, {1, 1}, {0, 0}};
    arix_npe_program_append(prog, load);
    ArixNPEInstruction halt = {ARIX_HALT, -1, -1, -1, 0, {0, 0}, {0, 0}};
    arix_npe_program_append(prog, halt);

    ArixNPEProgram* folded = arix_npe_jit_constant_fold(prog, NULL);
    ASSERT(folded != NULL, "folded with null memory");

    arix_npe_program_destroy(folded);
    arix_npe_program_destroy(prog);
}

int main(void) {
    run_test("constant_fold_identity", test_constant_fold_identity);
    run_test("constant_fold_add", test_constant_fold_add);
    run_test("constant_fold_null_memory", test_constant_fold_null_memory);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
