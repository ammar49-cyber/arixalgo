#ifndef ARIX_OBFUSCATION_ADVANCED_H
#define ARIX_OBFUSCATION_ADVANCED_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_OBF_MAX_BINARY_OPS 256
#define ARIX_OBF_MAX_IAT_ENTRIES 128
#define ARIX_OBF_MAX_VM_SLOTS 8

/* Binary-level instruction substitution */
typedef struct {
    uint8_t original_opcode;
    uint8_t substitute_opcode;
    uint8_t prefix_bytes[4];
    int prefix_count;
    uint8_t suffix_bytes[4];
    int suffix_count;
} ArixBinarySubstRule;

typedef struct {
    ArixBinarySubstRule rules[ARIX_OBF_MAX_BINARY_OPS];
    int rule_count;
} ArixBinarySubst;

int  arix_binary_subst_init(ArixBinarySubst* bs);
int  arix_binary_subst_add_rule(ArixBinarySubst* bs, uint8_t orig, uint8_t subst, const uint8_t* prefix, int pcount, const uint8_t* suffix, int scount);
int  arix_binary_subst_apply(ArixBinarySubst* bs, uint8_t* code, size_t* code_len, size_t max_len);

/* Junk code generation */
typedef struct {
    uint8_t junk_code[64][16];
    int junk_count;
} ArixJunkCodeGen;

int  arix_junk_code_init(ArixJunkCodeGen* jcg);
int  arix_junk_code_add_pattern(ArixJunkCodeGen* jcg, const uint8_t* pattern, size_t len);
int  arix_junk_code_insert(ArixJunkCodeGen* jcg, uint8_t* code, size_t* code_len, size_t max_len, int position);

/* Constant unfolding */
int  arix_constant_unfold_int32(uint32_t value, uint8_t* expr_out, size_t* expr_len);
int  arix_constant_unfold_int64(uint64_t value, uint8_t* expr_out, size_t* expr_len);

/* Array dimension obfuscation */
int  arix_array_obfuscate_indices(const size_t* dims, int ndim, size_t* linearized, size_t* obfuscated_indices, int n_indices);

/* Bogus control flow */
typedef struct {
    uintptr_t fake_entry;
    uintptr_t real_entry;
} ArixBogusCF;

int  arix_bogus_cf_init(ArixBogusCF* bcf);
int  arix_bogus_cf_add_fake_block(ArixBogusCF* bcf, const uint8_t* fake_code, size_t fake_len);
int  arix_bogus_cf_redirect(ArixBogusCF* bcf, uint8_t* code, size_t code_len);

/* Anti-hook (IAT protection) */
typedef struct {
    struct { const char* name; void* original; void* current; } entries[ARIX_OBF_MAX_IAT_ENTRIES];
    int count;
} ArixIATProtect;

int  arix_iat_protect_init(ArixIATProtect* iat);
int  arix_iat_protect_add_entry(ArixIATProtect* iat, const char* name, void* original);
int  arix_iat_protect_scan(ArixIATProtect* iat);
int  arix_iat_protect_restore(ArixIATProtect* iat);

/* White-box cryptography wrapper */
typedef struct {
    uint32_t te0[256],te1[256],te2[256],te3[256];
    uint32_t td0[256],td1[256],td2[256],td3[256];
    uint8_t embedded_key[16];
    int initialized;
} ArixWhiteBoxAES;

int  arix_whitebox_aes_init(ArixWhiteBoxAES* wb, const uint8_t key[16]);
void arix_whitebox_aes_encrypt(ArixWhiteBoxAES* wb, const uint8_t in[16], uint8_t out[16]);

/* Import address table obfuscation */
typedef struct {
    uint32_t api_hashes[ARIX_OBF_MAX_IAT_ENTRIES];
    void* resolved_ptrs[ARIX_OBF_MAX_IAT_ENTRIES];
    int count;
} ArixIATObfuscation;

int  arix_iat_obfuscation_init(ArixIATObfuscation* io);
uint32_t arix_iat_hash_name(const char* name);
void* arix_iat_resolve_by_hash(ArixIATObfuscation* io, uint32_t hash);

/* Exception handler obfuscation */
typedef struct {
    uintptr_t handler;
    uintptr_t next;
} ArixSEHObfuscation;

int  arix_seh_obfuscation_init(ArixSEHObfuscation* seh);
int  arix_seh_obfuscation_install(ArixSEHObfuscation* seh, void* handler);

/* TLS callback obfuscation */
int  arix_tls_callback_register(void (*cb)(void*, int, void*));
int  arix_tls_callback_obfuscate(void);

/* Anti-dump */
typedef struct {
    uint8_t section_hash[32];
    uintptr_t image_base;
    size_t image_size;
    int protected;
} ArixAntiDump;

int  arix_antidump_init(ArixAntiDump* ad);
int  arix_antidump_protect(ArixAntiDump* ad);
int  arix_antidump_verify(ArixAntiDump* ad);

/* Multi-VM diversity */
typedef struct {
    uint8_t vm_slots[ARIX_OBF_MAX_VM_SLOTS][4096];
    size_t vm_sizes[ARIX_OBF_MAX_VM_SLOTS];
    int current_slot;
} ArixMultiVM;

int  arix_multi_vm_init(ArixMultiVM* mvm);
int  arix_multi_vm_switch(ArixMultiVM* mvm);

/* Instruction scheduling */
int  arix_inst_schedule_randomize(uint8_t* code, size_t* code_len, size_t max_len);

#ifdef __cplusplus
}
#endif
#endif
