#ifndef ARIX_MODEL_CHECKING_H
#define ARIX_MODEL_CHECKING_H
/*
 * S8 Formal Verification — Model Checking Framework
 * Lightweight model checking for crypto primitives, data flow, and invariants.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_MODEL_MAX_STATES 1024
#define ARIX_MODEL_MAX_TRANSITIONS 4096
#define ARIX_MODEL_PROP_MAX_LEN 256

typedef struct {
    uint32_t state_id;
    uint32_t next_states[8];
    int next_count;
    int is_accepting;
    int is_error;
} ArixModelState;

typedef struct {
    ArixModelState states[ARIX_MODEL_MAX_STATES];
    int state_count;
    int initial_state;
    char property[ARIX_MODEL_PROP_MAX_LEN];
} ArixModel;

typedef struct {
    int total_states;
    int reachable_states;
    int deadlock_states;
    int error_states;
    int property_satisfied;
    char counterexample[ARIX_MODEL_PROP_MAX_LEN];
} ArixModelCheckResult;

int  arix_model_init(ArixModel* model);
int  arix_model_add_state(ArixModel* model, uint32_t state_id, int is_accepting, int is_error);
int  arix_model_add_transition(ArixModel* model, uint32_t from, uint32_t to);
int  arix_model_set_property(ArixModel* model, const char* property);
ArixModelCheckResult arix_model_check(ArixModel* model);
int  arix_model_verify_invariant(ArixModel* model, int (*invariant)(uint32_t state_id));

#ifdef __cplusplus
}
#endif
#endif
