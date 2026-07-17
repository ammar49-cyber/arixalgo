#ifndef SNEPPX_QUIC_TRANSPORT_H
#define SNEPPX_QUIC_TRANSPORT_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_quic_create_listener(const char* host, int port, const char* cert_path, const char* key_path);
void SNEPPX_quic_destroy_listener(void* listener);
int SNEPPX_quic_listen(void* listener, int backlog);
void* SNEPPX_quic_accept(void* listener, int timeout_ms);
void* SNEPPX_quic_dial(const char* host, int port, int timeout_ms);
void SNEPPX_quic_close(void* conn);
int SNEPPX_quic_send(void* conn, const unsigned char* data, size_t len);
int SNEPPX_quic_recv(void* conn, unsigned char* buf, size_t max_len, int timeout_ms);
int SNEPPX_quic_stream_open(void* conn, uint64_t* stream_id);
int SNEPPX_quic_stream_close(void* conn, uint64_t stream_id);
int SNEPPX_quic_stream_send(void* conn, uint64_t stream_id, const unsigned char* data, size_t len);
int SNEPPX_quic_stream_recv(void* conn, uint64_t stream_id, unsigned char* buf, size_t max_len, int timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
