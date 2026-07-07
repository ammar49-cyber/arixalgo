#ifndef ARIX_S4_EXTENSIONS_H
#define ARIX_S4_EXTENSIONS_H
/* S4 Network Security extensions: TLS 1.3 full handshake, Noise NK/XX/IK, QUIC,
   mTLS, OCSP stapling, CT, DoH, WireGuard, IP blocklist, NIDS, traffic analysis,
   rate limiting, port knocking, gRPC auth */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARIX_TLS13_MAX_EXTENSIONS 16
#define ARIX_NOISE_MAX_PATTERNS 8
#define ARIX_IP_BLOCKLIST_MAX 256

/* TLS 1.3 full handshake */
typedef struct {
    uint8_t random[32];
    uint8_t session_id[32];
    int session_id_len;
    uint16_t cipher_suites[16];
    int cipher_count;
    uint16_t supported_groups[8];
    int group_count;
    uint8_t* extensions[ARIX_TLS13_MAX_EXTENSIONS];
    size_t ext_lens[ARIX_TLS13_MAX_EXTENSIONS];
    int ext_count;
} ArixTLS13ClientHello;

typedef struct {
    int handshake_complete;
    uint8_t master_secret[48];
    uint8_t server_random[32];
} ArixTLS13Session;

int  arix_tls13_client_hello_init(ArixTLS13ClientHello* ch);
int  arix_tls13_server_hello_parse(ArixTLS13Session* sess, const uint8_t* data, size_t len);
int  arix_tls13_derive_keys(ArixTLS13Session* sess, const uint8_t* psk, size_t psk_len);

/* Noise protocol patterns NK, XX, IK */
typedef struct {
    int pattern; /* 0=NK, 1=XX, 2=IK */
    uint8_t s[32], e[32], rs[32], re[32];
    int initiator;
    int step;
} ArixNoiseHandshake;

int  arix_noise_init(ArixNoiseHandshake* nh, int pattern, int initiator);
int  arix_noise_write_msg(ArixNoiseHandshake* nh, uint8_t* msg, size_t* msg_len);
int  arix_noise_read_msg(ArixNoiseHandshake* nh, const uint8_t* msg, size_t msg_len);

/* QUIC connection manager */
typedef struct {
    int connection_id;
    uint8_t* stream_buffers[16];
    size_t stream_sizes[16];
    int stream_count;
    int established;
} ArixQUICConn;

int  arix_quic_conn_init(ArixQUICConn* qc);
int  arix_quic_conn_handshake(ArixQUICConn* qc, const uint8_t* params, size_t params_len);
int  arix_quic_stream_send(ArixQUICConn* qc, int stream_id, const uint8_t* data, size_t len);
int  arix_quic_stream_recv(ArixQUICConn* qc, int stream_id, uint8_t* data, size_t* len);

/* mTLS */
int  arix_mtls_authenticate(const uint8_t* cert_der, size_t cert_len, const uint8_t* key_der, size_t key_len);

/* OCSP stapling */
int  arix_ocsp_request(const uint8_t* issuer_cert, size_t issuer_len, const uint8_t* cert, size_t cert_len, uint8_t* response, size_t* resp_len);
int  arix_ocsp_verify(const uint8_t* response, size_t resp_len);

/* Certificate Transparency */
int  arix_ct_verify_sct(const uint8_t* sct, size_t sct_len, const uint8_t* cert, size_t cert_len);

/* DNS over HTTPS */
int  arix_doh_resolve(const char* hostname, uint8_t* ip_out, size_t* ip_len);

/* WireGuard */
typedef struct {
    uint8_t private_key[32];
    uint8_t public_key[32];
    uint8_t preshared_key[32];
    int established;
} ArixWireGuardSession;

int  arix_wireguard_init(ArixWireGuardSession* wg);
int  arix_wireguard_handshake(ArixWireGuardSession* wg, const uint8_t* peer_key, size_t key_len);

/* IP blocklist */
typedef struct {
    uint32_t networks[ARIX_IP_BLOCKLIST_MAX];
    uint32_t masks[ARIX_IP_BLOCKLIST_MAX];
    int count;
} ArixIPBlocklist;

int  arix_ip_blocklist_init(ArixIPBlocklist* bl);
int  arix_ip_blocklist_add(ArixIPBlocklist* bl, const char* cidr);
int  arix_ip_blocklist_check(ArixIPBlocklist* bl, uint32_t ip);

/* NIDS */
int  arix_nids_init(void);
int  arix_nids_analyze_packet(const uint8_t* packet, size_t len);

/* Traffic analysis mitigation */
int  arix_traffic_pad(uint8_t* data, size_t* len, size_t max_len, size_t block_size);

/* Connection rate limiting */
typedef struct {
    uint32_t connection_counts[256];
    uint64_t windows[256];
    int max_per_window;
} ArixRateLimiter;

int  arix_rate_limiter_init(ArixRateLimiter* rl, int max_per_window);
int  arix_rate_limiter_check(ArixRateLimiter* rl, uint32_t src_ip);

/* Port knocking */
int  arix_port_knock_sequence(const uint16_t* ports, int port_count);
int  arix_port_knock_verify(const uint16_t* received, int count, const uint16_t* expected, int expected_count);

/* gRPC auth interceptor */
typedef struct {
    uint8_t token[64];
    size_t token_len;
    int authenticated;
} ArixGRPCAuth;

int  arix_grpc_auth_init(ArixGRPCAuth* ga, const uint8_t* token, size_t token_len);
int  arix_grpc_auth_verify(ArixGRPCAuth* ga, const uint8_t* received_token, size_t token_len);

#ifdef __cplusplus
}
#endif
#endif
