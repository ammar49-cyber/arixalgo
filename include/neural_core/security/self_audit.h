#ifndef ARIX_SELF_AUDIT_H
#define ARIX_SELF_AUDIT_H
/*
 * S9 Penetration Testing — Self-Audit Framework
 * Automated security assessment, vulnerability scanning, and
 * compliance verification.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_AUDIT_MAX_CHECKS 128
#define ARIX_CHECK_DESC_LEN 256

typedef enum {
    ARIX_AUDIT_PASS = 0,
    ARIX_AUDIT_FAIL = 1,
    ARIX_AUDIT_WARN = 2,
    ARIX_AUDIT_INFO = 3,
} ArixAuditStatus;

typedef struct {
    char check_name[ARIX_CHECK_DESC_LEN];
    ArixAuditStatus status;
    char details[ARIX_CHECK_DESC_LEN];
} ArixAuditCheck;

typedef struct {
    ArixAuditCheck checks[ARIX_AUDIT_MAX_CHECKS];
    int check_count;
    int total_passed;
    int total_failed;
    int total_warnings;
    double security_score;
} ArixSelfAudit;

int  arix_self_audit_init(ArixSelfAudit* audit);
void arix_self_audit_destroy(ArixSelfAudit* audit);
int  arix_self_audit_add_check(ArixSelfAudit* audit, const char* name,
                                 ArixAuditStatus status, const char* details);
int  arix_self_audit_run_all(ArixSelfAudit* audit);
int  arix_self_audit_run_category(ArixSelfAudit* audit, const char* category);
double arix_self_audit_score(ArixSelfAudit* audit);
int  arix_self_audit_export_report(ArixSelfAudit* audit, const char* output_path);
int  arix_self_audit_check_crypto(ArixSelfAudit* audit);
int  arix_self_audit_check_memory(ArixSelfAudit* audit);
int  arix_self_audit_check_network(ArixSelfAudit* audit);
int  arix_self_audit_check_ai_safety(ArixSelfAudit* audit);

#ifdef __cplusplus
}
#endif
#endif
