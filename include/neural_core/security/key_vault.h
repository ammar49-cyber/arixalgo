#ifndef ARIX_KEY_VAULT_H
#define ARIX_KEY_VAULT_H
/*
 * S6 Security UI — Key Management Vault
 * Secure key generation, storage, rotation, and access auditing.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_VAULT_MAX_KEYS 64
#define ARIX_VAULT_KEY_LEN 32
#define ARIX_VAULT_ID_LEN 16

typedef struct {
    uint8_t id[ARIX_VAULT_ID_LEN];
    uint8_t key_data[ARIX_VAULT_KEY_LEN];
    uint64_t created_at;
    uint64_t expires_at;
    int is_active;
    int access_count;
} ArixVaultKey;

typedef struct {
    ArixVaultKey keys[ARIX_VAULT_MAX_KEYS];
    int key_count;
    int is_locked;
} ArixKeyVault;

int  arix_key_vault_init(ArixKeyVault* vault);
void arix_key_vault_destroy(ArixKeyVault* vault);
int  arix_key_vault_generate_key(ArixKeyVault* vault, uint8_t* key_id, uint64_t ttl_seconds);
int  arix_key_vault_get_key(ArixKeyVault* vault, const uint8_t* key_id, uint8_t* key_out);
int  arix_key_vault_rotate_key(ArixKeyVault* vault, const uint8_t* key_id);
int  arix_key_vault_revoke_key(ArixKeyVault* vault, const uint8_t* key_id);
int  arix_key_vault_lock(ArixKeyVault* vault);
int  arix_key_vault_unlock(ArixKeyVault* vault, const uint8_t* master_key);

#ifdef __cplusplus
}
#endif
#endif
