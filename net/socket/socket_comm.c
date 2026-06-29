/*
 * Socket Communication Implementation — SKELETON
 * VERSION: v1.0
 */

#include "socket_comm.h"
#include <stdlib.h>
#include <string.h>

ArixSocket* arix_socket_create(ArixSocketType type) {
    ArixSocket* sock = (ArixSocket*)calloc(1, sizeof(ArixSocket));
    if (sock) { sock->type = type; sock->fd = -1; }
    return sock;
}

void arix_socket_destroy(ArixSocket* sock) { free(sock); }

int arix_socket_bind(ArixSocket* sock, int port) {
    (void)sock; (void)port; return 0;
}
int arix_socket_listen(ArixSocket* sock, int backlog) {
    (void)sock; (void)backlog; return 0;
}
ArixSocket* arix_socket_accept(ArixSocket* server_sock) {
    (void)server_sock; return NULL;
}
int arix_socket_connect(ArixSocket* sock, const char* host, int port) {
    (void)sock; (void)host; (void)port; return 0;
}
int arix_socket_send(ArixSocket* sock, const void* data, size_t len) {
    (void)sock; (void)data; (void)len; return 0;
}
int arix_socket_recv(ArixSocket* sock, void* buf, size_t len) {
    (void)sock; (void)buf; (void)len; return 0;
}
int arix_socket_send_message(ArixSocket* sock, const void* data, size_t len) {
    (void)sock; (void)data; (void)len; return 0;
}
int arix_socket_recv_message(ArixSocket* sock, void** buf, size_t* len) {
    (void)sock; if (buf) *buf = NULL; if (len) *len = 0; return 0;
}
void arix_socket_close(ArixSocket* sock) { (void)sock; }
int arix_socket_send_tensor(ArixSocket* sock, const void* tensor_handle) {
    (void)sock; (void)tensor_handle; return 0;
}
int arix_socket_recv_tensor(ArixSocket* sock, void** tensor_handle) {
    (void)sock; if (tensor_handle) *tensor_handle = NULL; return 0;
}
int arix_socket_set_timeout(ArixSocket* sock, int ms) {
    (void)sock; (void)ms; return 0;
}
const char* arix_socket_error_string(int err) {
    (void)err; return "socket skeleton";
}
