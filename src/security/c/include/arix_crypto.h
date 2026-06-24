#ifndef ARIX_CRYPTO_H
#define ARIX_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

int arix_ed25519_verify(const unsigned char* message, size_t msg_len,
                        const unsigned char* signature, const unsigned char* public_key);

int arix_random_bytes(unsigned char* buffer, size_t len);

#endif /* ARIX_CRYPTO_H */
