#ifndef ARIX_TRANSPORT_SECURITY_H
#define ARIX_TRANSPORT_SECURITY_H
/*
 * S4 Network Security — Transport Security Layer
 * TLS 1.3 wrappers, Noise protocol handshake, QUIC session management.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_TLS_MAX_SESSIONS 64
#define ARIX_TLS_KEY_LEN 32
#define ARIX_TLS_NONCE_LEN 12

typedef struct {
    int session_id;
    int is_active;
    uint8_t session_key[ARIX_TLS_KEY_LEN];
    uint64_t creation_time;
    uint64_t last_used;
} ArixTLSSession;

typedef struct {
    int enabled;
    ArixTLSSession sessions[ARIX_TLS_MAX_SESSIONS];
    int session_count;
    int use_noise_protocol;
    int use_quic;
} ArixTransportSecurity;

int  arix_transport_init(ArixTransportSecurity* ts);
void arix_transport_shutdown(ArixTransportSecurity* ts);
int  arix_transport_new_session(ArixTransportSecurity* ts, const uint8_t* psk, size_t psk_len);
int  arix_transport_close_session(ArixTransportSecurity* ts, int session_id);
int  arix_transport_encrypt(ArixTransportSecurity* ts, int session_id,
                             const uint8_t* plaintext, size_t len,
                             uint8_t* ciphertext, uint8_t nonce[ARIX_TLS_NONCE_LEN]);
int  arix_transport_decrypt(ArixTransportSecurity* ts, int session_id,
                             const uint8_t* ciphertext, size_t len,
                             const uint8_t nonce[ARIX_TLS_NONCE_LEN],
                             uint8_t* plaintext);
int  arix_transport_noise_handshake(ArixTransportSecurity* ts,
                                     const uint8_t* prologue, size_t prologue_len,
                                     uint8_t* handshake_msg, size_t* msg_len);

#ifdef __cplusplus
}
#endif
#endif
