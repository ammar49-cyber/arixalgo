#ifndef ARIX_IDENTITY_MANAGEMENT_H
#define ARIX_IDENTITY_MANAGEMENT_H
/*
 * S4 Network Security — Identity & Access Management
 * Certificate pinning, certificate validation, DDoS protection.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_MAX_PINNED_CERTS 16
#define ARIX_CERT_FINGERPRINT_LEN 32

typedef struct {
    uint8_t fingerprint[ARIX_CERT_FINGERPRINT_LEN];
    char subject[256];
    uint64_t expiry;
    int is_active;
} ArixPinnedCert;

typedef struct {
    ArixPinnedCert certs[ARIX_MAX_PINNED_CERTS];
    int cert_count;
    int ddos_protection_enabled;
    uint64_t ddos_request_limit;
    uint64_t ddos_window_ms;
    uint64_t ddos_current_count;
    uint64_t ddos_window_start;
} ArixIdentityManager;

int  arix_identity_init(ArixIdentityManager* mgr);
void arix_identity_shutdown(ArixIdentityManager* mgr);
int  arix_identity_pin_cert(ArixIdentityManager* mgr, const uint8_t* fingerprint,
                             const char* subject, uint64_t expiry);
int  arix_identity_verify_cert(ArixIdentityManager* mgr, const uint8_t* fingerprint);
int  arix_identity_unpin_cert(ArixIdentityManager* mgr, const uint8_t* fingerprint);
int  arix_identity_ddos_check(ArixIdentityManager* mgr);
void arix_identity_ddos_reset(ArixIdentityManager* mgr);
int  arix_identity_tls_verify(const char* hostname, const uint8_t* cert_der, size_t cert_len);

#ifdef __cplusplus
}
#endif
#endif
