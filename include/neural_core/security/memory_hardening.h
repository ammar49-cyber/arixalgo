#ifndef ARIX_MEMORY_HARDENING_H
#define ARIX_MEMORY_HARDENING_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_MEM_GUARD_PAGE_SIZE 4096
#define ARIX_MEM_QUARANTINE_SIZE 128
#define ARIX_MAX_PAC_KEYS 16

/* Double-free / use-after-free detection */
typedef struct {
    void* entries[ARIX_MEM_QUARANTINE_SIZE];
    size_t sizes[ARIX_MEM_QUARANTINE_SIZE];
    uint64_t canaries[ARIX_MEM_QUARANTINE_SIZE];
    int count;
    int index;
} ArixMemQuarantine;

int  arix_mem_quarantine_init(ArixMemQuarantine* q);
int  arix_mem_quarantine_add(ArixMemQuarantine* q, void* ptr, size_t size);
int  arix_mem_quarantine_check(ArixMemQuarantine* q, const void* ptr);

/* Heap metadata encryption */
typedef struct {
    uint64_t xor_key;
    int enabled;
} ArixHeapMetadataEncrypt;

int  arix_heap_metadata_init(ArixHeapMetadataEncrypt* hme);
void arix_heap_metadata_encrypt(ArixHeapMetadataEncrypt* hme, void* metadata, size_t len);
void arix_heap_metadata_decrypt(ArixHeapMetadataEncrypt* hme, void* metadata, size_t len);

/* W^X enforcement */
int  arix_mem_enforce_wx(void* addr, size_t size);
int  arix_mem_set_rx(void* addr, size_t size);
int  arix_mem_set_rw(void* addr, size_t size);

/* Seccomp-BPF sandbox */
typedef struct {
    int enabled;
    int allow_read;
    int allow_write;
    int allow_open;
    int allow_socket;
    int allow_exec;
} ArixSeccompConfig;

int  arix_seccomp_init(ArixSeccompConfig* cfg);
int  arix_seccomp_apply(void);

/* Pointer authentication */
typedef struct {
    uint64_t pac_keys[ARIX_MAX_PAC_KEYS];
    int key_count;
} ArixPAC;

int  arix_pac_init(ArixPAC* pac);
uint64_t arix_pac_sign(ArixPAC* pac, const void* pointer, int key_idx);
int  arix_pac_verify(ArixPAC* pac, const void* pointer, uint64_t signature, int key_idx);

/* Control Flow Guard */
typedef struct {
    uintptr_t valid_targets[1024];
    int target_count;
} ArixCFG;

int  arix_cfg_init(ArixCFG* cfg);
int  arix_cfg_add_target(ArixCFG* cfg, void* target);
int  arix_cfg_validate(ArixCFG* cfg, void* target);

/* Safe stack (shadow call stack) */
#define ARIX_SHADOW_STACK_DEPTH 256

typedef struct {
    uintptr_t stack[ARIX_SHADOW_STACK_DEPTH];
    int sp;
    int overflow_detected;
} ArixShadowStack;

int  arix_shadow_stack_init(ArixShadowStack* ss);
int  arix_shadow_stack_push(ArixShadowStack* ss, uintptr_t return_addr);
int  arix_shadow_stack_pop(ArixShadowStack* ss, uintptr_t* return_addr);

/* Thread-local canary pool */
typedef struct {
    uint64_t canaries[64];
    int count;
} ArixThreadCanaryPool;

int  arix_tls_canary_pool_init(ArixThreadCanaryPool* pool);
uint64_t arix_tls_canary_alloc(ArixThreadCanaryPool* pool);
int  arix_tls_canary_check(ArixThreadCanaryPool* pool, uint64_t canary);

/* Guard page pool */
typedef struct {
    void* pages[64];
    size_t sizes[64];
    int count;
} ArixGuardPagePool;

int  arix_guard_pool_init(ArixGuardPagePool* pool);
void* arix_guard_pool_alloc(ArixGuardPagePool* pool, size_t size);
int  arix_guard_pool_free(ArixGuardPagePool* pool, void* ptr);

/* Memory pressure detection */
typedef struct {
    size_t total_allocated;
    size_t peak_allocated;
    size_t allocation_count;
    size_t allocation_limit;
    uint64_t last_warning_time;
    int pressure_level;
} ArixMemPressure;

int  arix_mem_pressure_init(ArixMemPressure* mp, size_t limit);
int  arix_mem_pressure_track(ArixMemPressure* mp, size_t size);
int  arix_mem_pressure_release(ArixMemPressure* mp, size_t size);
int  arix_mem_pressure_check(ArixMemPressure* mp);

#ifdef __cplusplus
}
#endif
#endif
