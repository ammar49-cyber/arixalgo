#ifndef ARIX_INTEGRITY_MONITOR_H
#define ARIX_INTEGRITY_MONITOR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ARIX_MONITOR_EVENT_TEXT_MODIFIED,
    ARIX_MONITOR_EVENT_CANARY_TRIGGERED,
    ARIX_MONITOR_EVENT_FUNC_PTR_MODIFIED,
    ARIX_MONITOR_EVENT_HEAP_CORRUPTION,
    ARIX_MONITOR_EVENT_HEARTBEAT_MISS,
    ARIX_MONITOR_EVENT_SELF_TAMPER,
    ARIX_MONITOR_EVENT_PATTERN_FOUND,
    ARIX_MONITOR_EVENT_FREQ_ANOMALY,
} ArixMonitorEventType;

typedef struct {
    ArixMonitorEventType type;
    const char*          description;
    uint64_t             address;
    size_t               size;
    uint64_t             timestamp;
} ArixMonitorEvent;

typedef void (*ArixMonitorCallback)(const ArixMonitorEvent* event);

int  arix_monitor_init(void);
void arix_monitor_shutdown(void);
int  arix_monitor_start(uint64_t interval_ms);
int  arix_monitor_stop(void);

int  arix_monitor_register_region(const char* name, const void* addr, size_t size);
int  arix_monitor_unregister_region(const char* name);

int  arix_monitor_verify_all(void);
int  arix_monitor_verify_region(const char* name);

int  arix_monitor_check_canary(void);
void arix_monitor_refresh_canary(void);

void arix_monitor_set_callback(ArixMonitorCallback cb);

int  arix_monitor_freq_analyze(void);
void arix_monitor_freq_reset(void);

void arix_monitor_timing_set_baseline(double mean, double stddev);
int  arix_monitor_timing_check(uint64_t elapsed_us);

int  arix_monitor_api_hook_check(void);
void arix_monitor_api_hook_enable(const void* base, size_t size);

int  arix_monitor_syscall_track(int syscall_num);
int  arix_monitor_syscall_analyze(void);
void arix_monitor_syscall_learn_baseline(void);
void arix_monitor_syscall_enable(void);

int  arix_monitor_verify_single_region(const char* name);
int  arix_monitor_set_canary(int depth);
int  arix_monitor_check_canary_at(int depth);
int  arix_monitor_get_events(ArixMonitorEvent* buffer, int max);
int  arix_monitor_scan_memory_for_pattern(const unsigned char* pattern, size_t pattern_len, const void* start, const void* end);
void arix_monitor_set_anomaly_threshold(int threshold);
int  arix_monitor_check_self(void);
int  arix_monitor_set_heartbeat(uint64_t interval_ms);
int  arix_monitor_add_callback(ArixMonitorCallback cb);
int  arix_monitor_remove_callback(ArixMonitorCallback cb);

#ifdef __cplusplus
}
#endif

#endif /* ARIX_INTEGRITY_MONITOR_H */
