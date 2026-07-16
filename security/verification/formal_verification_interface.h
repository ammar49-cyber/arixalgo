#ifndef SNEPPX_FORMAL_VERIFICATION_INTERFACE_H
#define SNEPPX_FORMAL_VERIFICATION_INTERFACE_H

#include <stdint.h>

#define SNEPPX_FV_MAX_STATES 1024
#define SNEPPX_FV_MAX_TRANSITIONS 4096

typedef struct {
    uint32_t state_id;
    uint32_t next_count;
    uint32_t next_states[8];
    int is_error;
    int is_accepting;
} SNEPPXFVState;

typedef struct {
    SNEPPXFVState states[SNEPPX_FV_MAX_STATES];
    int state_count;
    uint32_t initial_state;
} SNEPPXFormalModel;

int SNEPPX_fv_model_init(void);
int SNEPPX_fv_model_check_property(const char* property);
int SNEPPX_fv_verify_invariant(int (*invariant)(uint32_t));
int SNEPPX_fv_minimize(void);
int SNEPPX_fv_get_reachable_count(void);
int SNEPPX_fv_get_deadlock_count(void);

#endif
