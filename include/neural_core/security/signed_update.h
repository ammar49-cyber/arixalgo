#ifndef ARIX_SIGNED_UPDATE_H
#define ARIX_SIGNED_UPDATE_H
/*
 * S7 Secure Updates — Signed Update Verification
 * Cryptographic verification of signed delta updates with rollback protection.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_UPDATE_SIG_LEN 64
#define ARIX_UPDATE_HASH_LEN 32
#define ARIX_MAX_UPDATE_CHUNKS 256

typedef struct {
    uint32_t chunk_index;
    uint8_t chunk_hash[ARIX_UPDATE_HASH_LEN];
    uint32_t chunk_size;
} ArixUpdateChunk;

typedef struct {
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    uint8_t signature[ARIX_UPDATE_SIG_LEN];
    uint8_t update_hash[ARIX_UPDATE_HASH_LEN];
    ArixUpdateChunk chunks[ARIX_MAX_UPDATE_CHUNKS];
    int chunk_count;
    uint64_t timestamp;
    int is_delta;
} ArixSignedUpdate;

typedef struct {
    uint32_t current_version[3];
    uint32_t min_allowed_version[3];
    int rollback_protection_enabled;
    int verification_enabled;
} ArixUpdateVerifier;

int  arix_update_verifier_init(ArixUpdateVerifier* uv);
void arix_update_verifier_destroy(ArixUpdateVerifier* uv);
int  arix_update_verifier_set_min_version(ArixUpdateVerifier* uv,
                                           uint32_t major, uint32_t minor, uint32_t patch);
int  arix_update_verifier_check(ArixUpdateVerifier* uv, const ArixSignedUpdate* update);
int  arix_update_verifier_apply(ArixUpdateVerifier* uv, const ArixSignedUpdate* update,
                                 const uint8_t* update_data, size_t data_len);
int  arix_update_verifier_rollback_check(ArixUpdateVerifier* uv, uint32_t target_version[3]);

#ifdef __cplusplus
}
#endif
#endif
