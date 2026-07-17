#include "reinforcement_learning.h"
#include <stdlib.h>

void* SNEPPX_rl_create(size_t state_dim, size_t action_dim, size_t hidden_dim, float lr, float gamma) {
    (void)state_dim; (void)action_dim; (void)hidden_dim; (void)lr; (void)gamma;
    return NULL;
}

void SNEPPX_rl_destroy(void* agent) { free(agent); }

int SNEPPX_rl_select_action(void* agent, const float* state, float* action) {
    (void)agent; (void)state; (void)action;
    return 0;
}

int SNEPPX_rl_update(void* agent, const float* state, const float* action, float reward, const float* next_state, int done) {
    (void)agent; (void)state; (void)action; (void)reward; (void)next_state; (void)done;
    return 0;
}
