#ifndef SNEPPX_WEBSOCKET_CLIENT_H
#define SNEPPX_WEBSOCKET_CLIENT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_ws_connect(const char* url, int timeout_ms);
void SNEPPX_ws_disconnect(void* ws);
int SNEPPX_ws_send_text(void* ws, const char* text, size_t len);
int SNEPPX_ws_send_binary(void* ws, const unsigned char* data, size_t len);
int SNEPPX_ws_send_ping(void* ws);
int SNEPPX_ws_recv(void* ws, unsigned char** data, size_t* len, int* opcode, int timeout_ms);
int SNEPPX_ws_set_on_message(void* ws, void (*cb)(void* ws, int opcode, const unsigned char* data, size_t len));
int SNEPPX_ws_set_on_error(void* ws, void (*cb)(void* ws, int error_code));
int SNEPPX_ws_is_connected(void* ws);
#ifdef __cplusplus
}
#endif
#endif
