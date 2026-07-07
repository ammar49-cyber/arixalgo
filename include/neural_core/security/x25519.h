#ifndef ARIX_X25519_H
#define ARIX_X25519_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_X25519_KEY_SIZE 32
#define ARIX_X25519_SHARED_SIZE 32

void arix_x25519_clamp(uint8_t scalar[ARIX_X25519_KEY_SIZE]);
void arix_x25519_scalar_mult(uint8_t out[ARIX_X25519_KEY_SIZE], const uint8_t scalar[ARIX_X25519_KEY_SIZE], const uint8_t point[ARIX_X25519_KEY_SIZE]);
void arix_x25519_keygen(uint8_t public_key[ARIX_X25519_KEY_SIZE], uint8_t secret_key[ARIX_X25519_KEY_SIZE]);
int  arix_x25519_shared_secret(uint8_t shared[ARIX_X25519_SHARED_SIZE], const uint8_t secret_key[ARIX_X25519_KEY_SIZE], const uint8_t public_key[ARIX_X25519_KEY_SIZE]);

void arix_curve25519_basepoint(uint8_t out[ARIX_X25519_KEY_SIZE]);
int  arix_x25519_scalar_valid(const uint8_t scalar[ARIX_X25519_KEY_SIZE]);

#ifdef __cplusplus
}
#endif
#endif
