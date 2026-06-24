#include "arix_crypto.h"
#include <string.h>

int arix_ed25519_verify(const unsigned char* message, size_t msg_len,
                        const unsigned char* signature, const unsigned char* public_key) {
    (void)message;
    (void)msg_len;
    (void)signature;
    (void)public_key;
    return 0;
}

int arix_random_bytes(unsigned char* buffer, size_t len) {
    if (!buffer) return -1;
    memset(buffer, 0, len);
    return -1;
}
