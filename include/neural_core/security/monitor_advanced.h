#ifndef ARIX_MONITOR_ADVANCED_H
#define ARIX_MONITOR_ADVANCED_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_MON_MAX_REGIONS 128
#define ARIX_MON_MAX_EVENTS 1024
#define ARIX_MON_ML_FEATURES 8

/* Code segment tamper detection */
typedef struct {
    const void* code_addr;
    size_t code_size;
    uint8_t baseline_hash[32];
    int enabled;
} ArixCodeTamperDetector;

int  arix_code_tamper_init(ArixCodeTamperDetector* ctd, const void* addr, size_t size);
int  arix_code_tamper_check(ArixCodeTamperDetector* ctd);

/* Function pointer hook detection */
typedef struct {
    const void** func_ptrs[64];
    uintptr_t original_values[64];
    int count;
} ArixFuncPtrDetector;

int  arix_func_ptr_detector_init(ArixFuncPtrDetector* fpd);
int  arix_func_ptr_detector_watch(ArixFuncPtrDetector* fpd, const void** func_ptr);
int  arix_func_ptr_detector_scan(ArixFuncPtrDetector* fpd);

/* Heap corruption detector */
typedef struct {
    uint64_t sentinel_value;
    int enabled;
} ArixHeapCorruptionDetector;

int  arix_heap_corruption_init(ArixHeapCorruptionDetector* hcd);
int  arix_heap_corruption_apply_sentinel(ArixHeapCorruptionDetector* hcd, void* alloc, size_t size);
int  arix_heap_corruption_check(ArixHeapCorruptionDetector* hcd, void* alloc, size_t size);

/* Stack overflow detection */
int  arix_stack_overflow_guard_install(void);
int  arix_stack_overflow_check(void);

/* Return address verification */
int  arix_ret_addr_verify(void* ret_addr, void* expected_ret_addr);

/* Instruction-level tracing (stub: platform-specific) */
typedef struct {
    int enabled;
    uint64_t trace_buffer_size;
    void* trace_buffer;
} ArixInstructionTracer;

int  arix_inst_tracer_init(ArixInstructionTracer* tracer);
int  arix_inst_tracer_start(ArixInstructionTracer* tracer);
int  arix_inst_tracer_stop(ArixInstructionTracer* tracer);

/* ML anomaly detector */
typedef struct {
    double means[ARIX_MON_ML_FEATURES];
    double stds[ARIX_MON_ML_FEATURES];
    int trained;
    double threshold;
} ArixMLAnomalyDetector;

int  arix_ml_anomaly_init(ArixMLAnomalyDetector* ml);
int  arix_ml_anomaly_train(ArixMLAnomalyDetector* ml, const double features[][ARIX_MON_ML_FEATURES], int n_samples);
double arix_ml_anomaly_score(ArixMLAnomalyDetector* ml, const double features[ARIX_MON_ML_FEATURES]);
int  arix_ml_anomaly_is_anomaly(ArixMLAnomalyDetector* ml, const double features[ARIX_MON_ML_FEATURES]);

/* File system integrity */
typedef struct {
    char paths[64][256];
    uint8_t hashes[64][32];
    int count;
    int enabled;
} ArixFSIntegrity;

int  arix_fs_integrity_init(ArixFSIntegrity* fsi);
int  arix_fs_integrity_watch(ArixFSIntegrity* fsi, const char* path);
int  arix_fs_integrity_scan(ArixFSIntegrity* fsi);

/* Registry key monitoring (Windows) / file monitoring (Linux) */
int  arix_persistence_monitor_init(void);
int  arix_persistence_monitor_scan(void);

/* Process injection detection */
int  arix_proc_injection_detect(void);

/* Network connection monitoring */
int  arix_net_conn_monitor_init(void);
int  arix_net_conn_monitor_check(void);

/* USB/device insertion detection */
int  arix_device_insertion_detect(void);

/* Kernel object reference monitor */
int  arix_kernel_obj_monitor_init(void);
int  arix_kernel_obj_monitor_check(void);

/* TOCTOU detection */
typedef struct {
    uint8_t baseline[32];
    int initialized;
} ArixTOCTOUDetector;

int  arix_toctou_init(ArixTOCTOUDetector* td, const char* path);
int  arix_toctou_check(ArixTOCTOUDetector* td, const char* path);

/* IMA-style integrity */
int  arix_ima_measure(const char* path, uint8_t hash[32]);
int  arix_ima_appraise(const char* path, const uint8_t hash[32]);

/* Alert correlation engine */
typedef struct {
    struct { uint64_t timestamp; int type; const char* desc; } events[ARIX_MON_MAX_EVENTS];
    int count;
    int alerts_triggered;
} ArixAlertCorrelator;

int  arix_alert_correlator_init(ArixAlertCorrelator* ac);
int  arix_alert_correlator_add(ArixAlertCorrelator* ac, int type, const char* desc);
int  arix_alert_correlator_evaluate(ArixAlertCorrelator* ac);

#ifdef __cplusplus
}
#endif
#endif
