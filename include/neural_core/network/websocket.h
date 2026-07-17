#ifndef SNEPPX_NETWORK_WEBSOCKET_H
#define SNEPPX_NETWORK_WEBSOCKET_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { void* handle; char url[512]; int connected; } SNEPPXWebSocket;
int SNEPPX_ws_open(SNEPPXWebSocket* ws, const char* url);
void SNEPPX_ws_close(SNEPPXWebSocket* ws);
int SNEPPX_ws_send(SNEPPXWebSocket* ws, int opcode, const unsigned char* data, size_t len);
int SNEPPX_ws_recv(SNEPPXWebSocket* ws, int* opcode, unsigned char** data, size_t* len, int timeout_ms);
int SNEPPX_ws_ping(SNEPPXWebSocket* ws);
typedef void (*SNEPPXWsOnMessage)(SNEPPXWebSocket* ws, int opcode, const unsigned char* data, size_t len, void* userdata);
void SNEPPX_ws_set_callback(SNEPPXWebSocket* ws, SNEPPXWsOnMessage cb, void* userdata);
#ifdef __cplusplus
}
#endif
#endif
