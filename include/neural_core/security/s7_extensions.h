#ifndef ARIX_S7_EXTENSIONS_H
#define ARIX_S7_EXTENSIONS_H
/* S7 extensions: TUF compliance, bsdiff delta, A/B partitions, manifest
   verification, TPM attestation, canary rollout, offline bundles, dependency resolver */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_TUF_MAX_KEYS 8
#define TPM_PCR_COUNT 24

/* TUF (The Update Framework) */
typedef struct {
    uint8_t root_key[32];
    uint8_t targets_key[32];
    uint8_t snapshot_key[32];
    uint8_t timestamp_key[32];
    int initialized;
} ArixTUFMetadata;

int  arix_tuf_init(ArixTUFMetadata* tuf);
int  arix_tuf_sign_root(ArixTUFMetadata* tuf, const uint8_t* data, size_t len, uint8_t* sig, size_t* sig_len);
int  arix_tuf_verify_targets(ArixTUFMetadata* tuf, const uint8_t* targets_json, size_t len);

/* bsdiff delta generation */
int  arix_bsdiff(const uint8_t* old_data, size_t old_len, const uint8_t* new_data, size_t new_len, uint8_t* patch, size_t* patch_len);
int  arix_bspatch(const uint8_t* old_data, size_t old_len, const uint8_t* patch, size_t patch_len, uint8_t* new_data, size_t* new_len);

/* A/B partition management */
typedef struct {
    int active_slot;
    int inactive_slot;
    uint8_t slot_a_hash[32];
    uint8_t slot_b_hash[32];
    int swap_ready;
} ArixABPartition;

int  arix_ab_partition_init(ArixABPartition* ab);
int  arix_ab_partition_mark_good(ArixABPartition* ab, int slot);
int  arix_ab_partition_swap(ArixABPartition* ab);

/* Manifest verification */
int  arix_manifest_verify(const char* manifest_path, const uint8_t* signature, size_t sig_len);

/* TPM attestation */
int  arix_tpm_pcr_read(int pcr_index, uint8_t* out, size_t* out_len);
int  arix_tpm_quote(const uint8_t* nonce, size_t nonce_len, uint8_t* quote, size_t* quote_len);

/* Canary rollout */
typedef struct {
    int total_nodes;
    int canary_nodes;
    int promoted;
} ArixCanaryRollout;

int  arix_canary_rollout_init(ArixCanaryRollout* cr, int total, int canary);
int  arix_canary_rollout_promote(ArixCanaryRollout* cr);

/* Offline update bundle */
typedef struct {
    uint8_t bundle_hash[32];
    size_t bundle_size;
    int signed_offline;
} ArixOfflineBundle;

int  arix_offline_bundle_create(ArixOfflineBundle* ob, const uint8_t* data, size_t data_len, const uint8_t* signing_key, size_t key_len);

/* Dependency resolver */
typedef struct {
    char name[64];
    uint32_t version_major, version_minor, version_patch;
    int resolved;
} ArixDepResolver;

int  arix_dep_resolver_init(ArixDepResolver* dr);
int  arix_dep_resolver_add_dep(ArixDepResolver* dr, const char* name, uint32_t maj, uint32_t min, uint32_t pat);
int  arix_dep_resolver_resolve(ArixDepResolver* dr);

#ifdef __cplusplus
}
#endif
#endif
