#ifndef ARIX_S8_EXTENSIONS_H
#define ARIX_S8_EXTENSIONS_H
/* S8 extensions: TLA+ parser, LTL model checker, symbolic execution,
   loop invariant inference, data flow analysis, Lean 4 proof export */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_TLA_MAX_SPEC 4096
#define ARIX_LTL_MAX_FORMULA 256
#define ARIX_SYMEX_MAX_PATHS 1024

/* TLA+ specification parser (simplified) */
typedef struct {
    char spec[ARIX_TLA_MAX_SPEC];
    int parsed;
    int state_count;
} ArixTLAParser;

int  arix_tla_parse(ArixTLAParser* parser, const char* spec_text);

/* LTL property verifier */
typedef struct {
    char formula[ARIX_LTL_MAX_FORMULA];
    int holds;
} ArixLTLVerifier;

int  arix_ltl_init(ArixLTLVerifier* ltl, const char* formula);
int  arix_ltl_check(ArixLTLVerifier* ltl, int* trace, int trace_len);

/* Symbolic execution engine */
typedef struct {
    uint64_t explored_paths;
    uint64_t bounded_paths;
    int depth_limit;
} ArixSymExEngine;

int  arix_symex_init(ArixSymExEngine* se, int depth_limit);
int  arix_symex_explore(ArixSymExEngine* se, const uint8_t* bytecode, size_t bc_len);

/* Loop invariant inference */
int  arix_loop_invariant_infer(const char* loop_body, char* invariant_out, size_t inv_size);

/* Data flow analysis */
typedef struct {
    int taint_marks[256];
    int taint_count;
} ArixDataFlow;

int  arix_data_flow_init(ArixDataFlow* df);
int  arix_data_flow_taint(ArixDataFlow* df, int var_id);
int  arix_data_flow_propagate(ArixDataFlow* df);

/* Lean 4 proof export */
int  arix_lean_export_proof(const char* theorem_name, const char* proof_body, const char* output_path);

#ifdef __cplusplus
}
#endif
#endif
