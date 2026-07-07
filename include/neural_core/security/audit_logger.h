#ifndef ARIX_AUDIT_LOGGER_H
#define ARIX_AUDIT_LOGGER_H
/*
 * S6 Security UI — Audit Logger
 * Tamper-evident audit log for security events, key access, and violations.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_AUDIT_MAX_ENTRIES 1024
#define ARIX_AUDIT_DESC_LEN 256

typedef struct {
    uint64_t timestamp;
    int event_type;
    char description[ARIX_AUDIT_DESC_LEN];
    uint64_t related_address;
    uint32_t crc;
} ArixAuditEntry;

typedef struct {
    ArixAuditEntry entries[ARIX_AUDIT_MAX_ENTRIES];
    int entry_count;
    int enabled;
    const char* log_file_path;
    uint32_t chain_crc;
} ArixAuditLogger;

int  arix_audit_init(ArixAuditLogger* audit, const char* log_path);
void arix_audit_shutdown(ArixAuditLogger* audit);
int  arix_audit_log(ArixAuditLogger* audit, int event_type,
                     const char* description, uint64_t related_address);
int  arix_audit_export(ArixAuditLogger* audit, const char* output_path);
int  arix_audit_verify_chain(ArixAuditLogger* audit);
int  arix_audit_search(ArixAuditLogger* audit, int event_type,
                        ArixAuditEntry* results, int max_results);

#ifdef __cplusplus
}
#endif
#endif
