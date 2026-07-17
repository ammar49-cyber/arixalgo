#ifndef SNEPPX_TLS_TRANSPORT_H
#define SNEPPX_TLS_TRANSPORT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { void* ctx; void* ssl; int fd; int is_server; } SNEPPXTlsSocket;
int SNEPPX_tls_init(void);
void SNEPPX_tls_cleanup(void);
int SNEPPX_tls_server_ctx_create(const char* cert_path, const char* key_path, void** ctx);
int SNEPPX_tls_client_ctx_create(void** ctx);
void SNEPPX_tls_ctx_destroy(void* ctx);
int SNEPPX_tls_accept(void* ctx, int client_fd, SNEPPXTlsSocket* sock);
int SNEPPX_tls_connect(void* ctx, const char* host, int port, SNEPPXTlsSocket* sock);
void SNEPPX_tls_close(SNEPPXTlsSocket* sock);
int SNEPPX_tls_send(SNEPPXTlsSocket* sock, const unsigned char* data, size_t len);
int SNEPPX_tls_recv(SNEPPXTlsSocket* sock, unsigned char* buf, size_t max_len);
#ifdef __cplusplus
}
#endif
#endif
