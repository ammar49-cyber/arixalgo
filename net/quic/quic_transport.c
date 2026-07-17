#include "quic_transport.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_quic_create_listener(const char* host, int port, const char* cert_path, const char* key_path) { (void)host; (void)port; (void)cert_path; (void)key_path; return NULL; }
void SNEPPX_quic_destroy_listener(void* listener) { free(listener); }
int SNEPPX_quic_listen(void* listener, int backlog) { (void)listener; (void)backlog; return 0; }
void* SNEPPX_quic_accept(void* listener, int timeout_ms) { (void)listener; (void)timeout_ms; return NULL; }
void* SNEPPX_quic_dial(const char* host, int port, int timeout_ms) { (void)host; (void)port; (void)timeout_ms; return NULL; }
void SNEPPX_quic_close(void* conn) { free(conn); }
int SNEPPX_quic_send(void* conn, const unsigned char* data, size_t len) { (void)conn; (void)data; (void)len; return 0; }
int SNEPPX_quic_recv(void* conn, unsigned char* buf, size_t max_len, int timeout_ms) { (void)conn; (void)buf; (void)max_len; (void)timeout_ms; return 0; }
int SNEPPX_quic_stream_open(void* conn, unsigned long long* stream_id) { (void)conn; (void)stream_id; return 0; }
int SNEPPX_quic_stream_close(void* conn, unsigned long long stream_id) { (void)conn; (void)stream_id; return 0; }
int SNEPPX_quic_stream_send(void* conn, unsigned long long stream_id, const unsigned char* data, size_t len) { (void)conn; (void)stream_id; (void)data; (void)len; return 0; }
int SNEPPX_quic_stream_recv(void* conn, unsigned long long stream_id, unsigned char* buf, size_t max_len, int timeout_ms) { (void)conn; (void)stream_id; (void)buf; (void)max_len; (void)timeout_ms; return 0; }
