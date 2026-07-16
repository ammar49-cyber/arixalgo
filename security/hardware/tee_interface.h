#ifndef SNEPPX_HARDWARE_SECURITY_INTERFACE_H
#define SNEPPX_HARDWARE_SECURITY_INTERFACE_H

#include <stdint.h>

#define SNEPPX_TEE_MAX_SLOTS 16
#define SNEPPX_TEE_KEY_SIZE 32

typedef enum {
    SNEPPX_TEE_NONE = 0,
    SNEPPX_TEE_SGX,
    SNEPPX_TEE_SEV,
    SNEPPX_TEE_TDX,
    SNEPPX_TEE_PSP
} SNEPPXTeeType;

typedef struct {
    int initialized;
    SNEPPXTeeType tee_type;
    int attestation_verified;
    int secure_boot_enabled;
    int hsm_present;
} SNEPPXHardwareSecurity;

int SNEPPX_hsm_attest(void);
int SNEPPX_hsm_seal_key(const uint8_t* key, size_t key_len, uint8_t* sealed, size_t* sealed_len);
int SNEPPX_hsm_unseal_key(const uint8_t* sealed, size_t sealed_len, uint8_t* key, size_t* key_len);
int SNEPPX_hsm_verify_measurement(const uint8_t* expected, size_t len);
int SNEPPX_hsm_get_random(uint8_t* buf, size_t len);

#endif
