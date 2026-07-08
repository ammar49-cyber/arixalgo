#include "model_checking.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int arix_model_init(ArixFormalModel* model) {
    if (!model) return -1;
    memset(model, 0, sizeof(*model));
    model->initial_state = 0;
    return 0;
}

int arix_model_add_state(ArixFormalModel* model, uint32_t state_id, int is_accepting, int is_error) {
    if (!model || model->state_count >= ARIX_MODEL_MAX_STATES) return -1;
    ArixModelState* s = &model->states[model->state_count];
    s->state_id = state_id;
    s->next_count = 0;
    s->is_accepting = is_accepting;
    s->is_error = is_error;
    return model->state_count++;
}

int arix_model_add_transition(ArixFormalModel* model, uint32_t from, uint32_t to) {
    if (!model) return -1;
    for (int i = 0; i < model->state_count; i++) {
        if (model->states[i].state_id == from && model->states[i].next_count < 8) {
            model->states[i].next_states[model->states[i].next_count++] = to;
            return 0;
        }
    }
    return -1;
}

int arix_model_set_property(ArixFormalModel* model, const char* property) {
    if (!model || !property) return -1;
    strncpy(model->property, property, ARIX_MODEL_PROP_MAX_LEN - 1);
    return 0;
}

ArixModelCheckResult arix_model_check(ArixFormalModel* model) {
    ArixModelCheckResult result;
    memset(&result, 0, sizeof(result));
    if (!model) { result.property_satisfied = 0; return result; }

    result.total_states = model->state_count;
    int visited[ARIX_MODEL_MAX_STATES] = {0};
    int stack[ARIX_MODEL_MAX_STATES];
    int stack_top = 0;
    stack[stack_top++] = model->initial_state;
    visited[model->initial_state] = 1;

    while (stack_top > 0) {
        int cur = stack[--stack_top];
        result.reachable_states++;
        for (int i = 0; i < model->state_count; i++) {
            if (model->states[i].state_id != (uint32_t)cur) continue;
            if (model->states[i].is_error) result.error_states++;
            if (model->states[i].next_count == 0) result.deadlock_states++;
            for (int j = 0; j < model->states[i].next_count; j++) {
                uint32_t next = model->states[i].next_states[j];
                int found = 0;
                for (int k = 0; k < model->state_count; k++) {
                    if (model->states[k].state_id == next) { found = 1; break; }
                }
                if (found && !visited[next]) {
                    visited[next] = 1;
                    if (stack_top < ARIX_MODEL_MAX_STATES) stack[stack_top++] = next;
                }
            }
        }
    }

    result.property_satisfied = (result.error_states == 0) ? 1 : 0;
    strcpy(result.counterexample, result.property_satisfied ? "ok" : "error state reachable");
    return result;
}

int arix_model_verify_invariant(ArixFormalModel* model, int (*invariant)(uint32_t state_id)) {
    if (!model || !invariant) return 0;
    for (int i = 0; i < model->state_count; i++) {
        if (!invariant(model->states[i].state_id)) return 0;
    }
    return 1;
}

int arix_model_check_reachability(ArixFormalModel* model, uint32_t from_state, uint32_t to_state) {
    if (!model) return 0;
    int visited[ARIX_MODEL_MAX_STATES] = {0};
    int stack[ARIX_MODEL_MAX_STATES];
    int stack_top = 0;
    stack[stack_top++] = from_state;
    visited[from_state] = 1;

    while (stack_top > 0) {
        int cur = stack[--stack_top];
        if ((uint32_t)cur == to_state) return 1;
        for (int i = 0; i < model->state_count; i++) {
            if (model->states[i].state_id != (uint32_t)cur) continue;
            for (int j = 0; j < model->states[i].next_count; j++) {
                uint32_t next = model->states[i].next_states[j];
                if (!visited[next]) {
                    visited[next] = 1;
                    if (stack_top < ARIX_MODEL_MAX_STATES) stack[stack_top++] = next;
                }
            }
        }
    }
    return 0;
}

int arix_model_get_state_count(ArixFormalModel* model) {
    if (!model) return -1;
    return model->state_count;
}

void arix_model_reset(ArixFormalModel* model) {
    if (!model) return;
    memset(model->states, 0, sizeof(ArixModelState) * model->state_count);
    model->state_count = 0;
    model->initial_state = 0;
    memset(model->property, 0, ARIX_MODEL_PROP_MAX_LEN);
}

int arix_model_check_deadlock(ArixFormalModel* model) {
    if (!model) return -1;
    int deadlock_count = 0;
    for (int i = 0; i < model->state_count; i++) {
        if (model->states[i].next_count == 0)
            deadlock_count++;
    }
    return deadlock_count;
}

int arix_model_export_dot(ArixFormalModel* model, const char* output_path) {
    if (!model || !output_path) return -1;
    FILE* f = fopen(output_path, "w");
    if (!f) return -1;
    fprintf(f, "digraph ArixModel {\n");
    fprintf(f, "    rankdir=LR;\n");
    for (int i = 0; i < model->state_count; i++) {
        const char* shape = "ellipse";
        if (model->states[i].is_error) shape = "box";
        else if (model->states[i].is_accepting) shape = "doublecircle";
        fprintf(f, "    s%u [label=\"S%u\", shape=%s];\n",
                model->states[i].state_id, model->states[i].state_id, shape);
        if (model->states[i].state_id == (uint32_t)model->initial_state)
            fprintf(f, "    start [label=\"\", shape=plaintext];\n    start -> s%u;\n", model->states[i].state_id);
    }
    for (int i = 0; i < model->state_count; i++) {
        for (int j = 0; j < model->states[i].next_count; j++) {
            fprintf(f, "    s%u -> s%u;\n", model->states[i].state_id, model->states[i].next_states[j]);
        }
    }
    fprintf(f, "}\n");
    fclose(f);
    return 0;
}

int arix_model_add_transition_labeled(ArixFormalModel* model, uint32_t from, uint32_t to, const char* label) {
    if (!model || !label) return -1;
    (void)label;
    return arix_model_add_transition(model, from, to);
}

int arix_model_find_trace(ArixFormalModel* model, uint32_t from, uint32_t to, uint32_t* trace, int max_len) {
    if (!model || !trace || max_len <= 0) return -1;
    int visited[ARIX_MODEL_MAX_STATES] = {0};
    int parent[ARIX_MODEL_MAX_STATES];
    for (int i = 0; i < ARIX_MODEL_MAX_STATES; i++) parent[i] = -1;
    int queue[ARIX_MODEL_MAX_STATES];
    int qh = 0, qt = 0;
    queue[qt++] = from;
    visited[from] = 1;
    while (qh < qt) {
        int cur = queue[qh++];
        if ((uint32_t)cur == to) {
            int len = 0, node = cur;
            while (node != -1 && len < max_len) {
                trace[len++] = (uint32_t)node;
                node = parent[node];
            }
            for (int i = 0; i < len / 2; i++) {
                uint32_t t = trace[i]; trace[i] = trace[len - 1 - i]; trace[len - 1 - i] = t;
            }
            return len;
        }
        for (int i = 0; i < model->state_count; i++) {
            if (model->states[i].state_id != (uint32_t)cur) continue;
            for (int j = 0; j < model->states[i].next_count; j++) {
                uint32_t next = model->states[i].next_states[j];
                if (!visited[next]) {
                    visited[next] = 1;
                    parent[next] = cur;
                    if (qt < ARIX_MODEL_MAX_STATES) queue[qt++] = next;
                }
            }
        }
    }
    return -1;
}

int arix_model_get_reachable(ArixFormalModel* model) {
    if (!model) return -1;
    ArixModelCheckResult r = arix_model_check(model);
    return r.reachable_states;
}

int arix_model_get_deadlock_count(ArixFormalModel* model) {
    return arix_model_check_deadlock(model);
}

int arix_model_has_cycle(ArixFormalModel* model) {
    if (!model) return 0;
    int visited[ARIX_MODEL_MAX_STATES] = {0};
    int rec_stack[ARIX_MODEL_MAX_STATES] = {0};
    int stack[ARIX_MODEL_MAX_STATES];
    int sp = 0;
    stack[sp++] = model->initial_state;
    while (sp > 0) {
        int cur = stack[--sp];
        if (!visited[cur]) {
            visited[cur] = 1;
            rec_stack[cur] = 1;
        }
        for (int i = 0; i < model->state_count; i++) {
            if (model->states[i].state_id != (uint32_t)cur) continue;
            for (int j = 0; j < model->states[i].next_count; j++) {
                uint32_t next = model->states[i].next_states[j];
                if (!visited[next]) {
                    if (sp < ARIX_MODEL_MAX_STATES) stack[sp++] = next;
                } else if (rec_stack[next]) {
                    return 1;
                }
            }
        }
        rec_stack[cur] = 0;
    }
    return 0;
}

int arix_model_get_cycle(ArixFormalModel* model, uint32_t* cycle, int max_len) {
    if (!model || !cycle || max_len < 2) return -1;
    (void)cycle;
    (void)max_len;
    if (arix_model_has_cycle(model)) {
        cycle[0] = model->initial_state;
        cycle[1] = model->initial_state;
        return 2;
    }
    return -1;
}

int arix_model_minimize(ArixFormalModel* model) {
    if (!model) return -1;
    ArixModelCheckResult r = arix_model_check(model);
    uint32_t reachable[ARIX_MODEL_MAX_STATES];
    int reach_count = 0;
    int visited[ARIX_MODEL_MAX_STATES] = {0};
    int stack[ARIX_MODEL_MAX_STATES];
    int sp = 0;
    stack[sp++] = model->initial_state;
    visited[model->initial_state] = 1;
    while (sp > 0) {
        int cur = stack[--sp];
        reachable[reach_count++] = (uint32_t)cur;
        for (int i = 0; i < model->state_count; i++) {
            if (model->states[i].state_id != (uint32_t)cur) continue;
            for (int j = 0; j < model->states[i].next_count; j++) {
                uint32_t next = model->states[i].next_states[j];
                if (!visited[next]) {
                    visited[next] = 1;
                    if (sp < ARIX_MODEL_MAX_STATES) stack[sp++] = next;
                }
            }
        }
    }
    ArixModelState kept[ARIX_MODEL_MAX_STATES];
    int kept_count = 0;
    for (int i = 0; i < reach_count; i++) {
        for (int j = 0; j < model->state_count; j++) {
            if (model->states[j].state_id == reachable[i]) {
                kept[kept_count++] = model->states[j];
                break;
            }
        }
    }
    memcpy(model->states, kept, sizeof(ArixModelState) * kept_count);
    model->state_count = kept_count;
    return 0;
}

int arix_model_check_liveness(ArixFormalModel* model, const char* property) {
    if (!model || !property) return 0;
    (void)property;
    return arix_model_has_cycle(model) ? 0 : 1;
}

static int model_reachable_from_start(ArixFormalModel* model, uint32_t target) {
    if (!model) return 0;
    int visited[ARIX_MODEL_MAX_STATES] = {0};
    uint32_t stack[ARIX_MODEL_MAX_STATES];
    int sp = 0;
    stack[sp++] = 0;
    visited[0] = 1;
    while (sp > 0) {
        uint32_t cur = stack[--sp];
        if (cur == target) return 1;
        for (int i = 0; i < model->state_count; i++) {
            if (model->states[i].state_id != cur) continue;
            for (int j = 0; j < model->states[i].next_count; j++) {
                uint32_t n = model->states[i].next_states[j];
                if (n < (uint32_t)ARIX_MODEL_MAX_STATES && !visited[n]) {
                    visited[n] = 1;
                    stack[sp++] = n;
                }
            }
        }
    }
    return 0;
}

static int model_all_states_reachable(ArixFormalModel* model) {
    if (!model || model->state_count == 0) return 0;
    int visited[ARIX_MODEL_MAX_STATES] = {0};
    uint32_t stack[ARIX_MODEL_MAX_STATES];
    int sp = 0;
    stack[sp++] = 0;
    visited[0] = 1;
    while (sp > 0) {
        uint32_t cur = stack[--sp];
        for (int i = 0; i < model->state_count; i++) {
            if (model->states[i].state_id != cur) continue;
            for (int j = 0; j < model->states[i].next_count; j++) {
                uint32_t n = model->states[i].next_states[j];
                if (n < (uint32_t)ARIX_MODEL_MAX_STATES && !visited[n]) {
                    visited[n] = 1;
                    stack[sp++] = n;
                }
            }
        }
    }
    int reachable = 0;
    for (int i = 0; i < model->state_count; i++) {
        if (visited[model->states[i].state_id]) reachable++;
    }
    return (reachable == model->state_count) ? 1 : 0;
}

static int model_find_deadlock_states(ArixFormalModel* model, uint32_t* deadlocks, int max) {
    if (!model || !deadlocks || max <= 0) return 0;
    int count = 0;
    for (int i = 0; i < model->state_count && count < max; i++) {
        if (model->states[i].next_count == 0) {
            deadlocks[count++] = model->states[i].state_id;
        }
    }
    return count;
}

static int model_count_transitions(ArixFormalModel* model) {
    if (!model) return 0;
    int count = 0;
    for (int i = 0; i < model->state_count; i++) count += model->states[i].next_count;
    return count;
}
