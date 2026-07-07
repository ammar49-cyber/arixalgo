#ifndef ARIX_HMAC_H
#define ARIX_HMAC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_HMAC_MAX_OUTPUT 64
#define ARIX_HMAC_MAX_KEY 128

typedef struct {
    uint8_t key[ARIX_HMAC_MAX_KEY];
    size_t key_len;
    int hash_type;
} ArixHMAC;

int  arix_hmac_init(ArixHMAC* ctx, const uint8_t* key, size_t key_len, int hash_type);
int  arix_hmac_compute(ArixHMAC* ctx, const uint8_t* data, size_t data_len, uint8_t* out, size_t* out_len);
int  arix_hmac_sha256(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t out[32]);
int  arix_hmac_sha512(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t out[64]);

#ifdef __cplusplus
}
#endif
#endif
