#include "formal_verification_interface.h"
#include <string.h>
#include <stdlib.h>

static SNEPPXFormalModel g_fv_model;
static int g_fv_initialized = 0;

int SNEPPX_fv_model_init(void) {
    memset(&g_fv_model, 0, sizeof(g_fv_model));
    g_fv_initialized = 1;
    return 0;
}

int SNEPPX_fv_model_check_property(const char* property) {
    if (!property || !g_fv_initialized) return -1;
    (void)property;
    return 0;
}

int SNEPPX_fv_verify_invariant(int (*invariant)(uint32_t)) {
    if (!invariant || !g_fv_initialized) return 0;
    for (int i = 0; i < g_fv_model.state_count; i++) {
        if (!invariant(g_fv_model.states[i].state_id)) return 0;
    }
    return 1;
}

int SNEPPX_fv_minimize(void) {
    if (!g_fv_initialized) return -1;
    return 0;
}

int SNEPPX_fv_get_reachable_count(void) {
    if (!g_fv_initialized) return -1;
    return g_fv_model.state_count;
}

int SNEPPX_fv_get_deadlock_count(void) {
    if (!g_fv_initialized) return 0;
    int deadlocks = 0;
    for (int i = 0; i < g_fv_model.state_count; i++) {
        if (g_fv_model.states[i].next_count == 0) deadlocks++;
    }
    return deadlocks;
}
