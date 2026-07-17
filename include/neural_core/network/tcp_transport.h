#ifndef SNEPPX_TCP_TRANSPORT_H
#define SNEPPX_TCP_TRANSPORT_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { char host[256]; int port; int fd; int is_listener; } SNEPPXTcpSocket;
int SNEPPX_tcp_listen(int port, int backlog, SNEPPXTcpSocket* sock);
int SNEPPX_tcp_accept(SNEPPXTcpSocket* listener, SNEPPXTcpSocket* client, int timeout_ms);
int SNEPPX_tcp_connect(const char* host, int port, int timeout_ms, SNEPPXTcpSocket* sock);
void SNEPPX_tcp_close(SNEPPXTcpSocket* sock);
int SNEPPX_tcp_send(SNEPPXTcpSocket* sock, const unsigned char* data, size_t len, int timeout_ms);
int SNEPPX_tcp_recv(SNEPPXTcpSocket* sock, unsigned char* buf, size_t max_len, int timeout_ms);
int SNEPPX_tcp_set_nodelay(SNEPPXTcpSocket* sock, int enable);
int SNEPPX_tcp_set_keepalive(SNEPPXTcpSocket* sock, int enable, int idle_s, int interval_s, int count);
#ifdef __cplusplus
}
#endif
#endif
