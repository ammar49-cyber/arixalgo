#ifndef ARIX_AES_GCM_H
#define ARIX_AES_GCM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_AES_BLOCK_SIZE 16
#define ARIX_AES256_KEY_SIZE 32
#define ARIX_GCM_IV_SIZE 12
#define ARIX_GCM_TAG_SIZE 16
#define ARIX_GCM_MAX_AAD 65536
#define ARIX_GCM_MAX_PLAINTEXT (1ULL << 36)

typedef struct {
    uint32_t rk[60];
    int rounds;
    uint8_t h[ARIX_AES_BLOCK_SIZE];
    uint8_t j0[ARIX_AES_BLOCK_SIZE];
    uint8_t tag[ARIX_GCM_TAG_SIZE];
    int mode;
} ArixAESGCM;

void arix_aes256_key_expansion(const uint8_t key[ARIX_AES256_KEY_SIZE], uint32_t rk[60]);
void arix_aes256_encrypt_block(const uint32_t rk[60], const uint8_t in[ARIX_AES_BLOCK_SIZE], uint8_t out[ARIX_AES_BLOCK_SIZE]);
void arix_aes256_decrypt_block(const uint32_t rk[60], const uint8_t in[ARIX_AES_BLOCK_SIZE], uint8_t out[ARIX_AES_BLOCK_SIZE]);

int  arix_aes_gcm_init(ArixAESGCM* ctx, const uint8_t key[ARIX_AES256_KEY_SIZE], const uint8_t iv[ARIX_GCM_IV_SIZE], int encrypt);
void arix_aes_gcm_update_aad(ArixAESGCM* ctx, const uint8_t* aad, size_t aad_len);
void arix_aes_gcm_encrypt(ArixAESGCM* ctx, const uint8_t* plaintext, uint8_t* ciphertext, size_t len);
int  arix_aes_gcm_decrypt(ArixAESGCM* ctx, const uint8_t* ciphertext, uint8_t* plaintext, size_t len);
void arix_aes_gcm_finalize(ArixAESGCM* ctx, uint8_t tag[ARIX_GCM_TAG_SIZE]);
int  arix_aes_gcm_verify_tag(ArixAESGCM* ctx, const uint8_t expected_tag[ARIX_GCM_TAG_SIZE]);

#ifdef __cplusplus
}
#endif
#endif
