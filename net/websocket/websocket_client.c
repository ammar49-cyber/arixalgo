#include "websocket_client.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_ws_connect(const char* url, int timeout_ms) { (void)url; (void)timeout_ms; return NULL; }
void SNEPPX_ws_disconnect(void* ws) { free(ws); }
int SNEPPX_ws_send_text(void* ws, const char* text, size_t len) { (void)ws; (void)text; (void)len; return 0; }
int SNEPPX_ws_send_binary(void* ws, const unsigned char* data, size_t len) { (void)ws; (void)data; (void)len; return 0; }
int SNEPPX_ws_send_ping(void* ws) { (void)ws; return 0; }
int SNEPPX_ws_recv(void* ws, unsigned char** data, size_t* len, int* opcode, int timeout_ms) { (void)ws; (void)data; (void)len; (void)opcode; (void)timeout_ms; return 0; }
int SNEPPX_ws_set_on_message(void* ws, void (*cb)(void* ws, int opcode, const unsigned char* data, size_t len)) { (void)ws; (void)cb; return 0; }
int SNEPPX_ws_set_on_error(void* ws, void (*cb)(void* ws, int error_code)) { (void)ws; (void)cb; return 0; }
int SNEPPX_ws_is_connected(void* ws) { (void)ws; return 0; }
