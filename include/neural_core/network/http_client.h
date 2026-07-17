#ifndef SNEPPX_HTTP_CLIENT_H
#define SNEPPX_HTTP_CLIENT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { char host[256]; int port; int use_ssl; void* conn; } SNEPPXHttpClient;
typedef struct { int status_code; char* headers; char* body; size_t body_len; } SNEPPXHttpResponse;
int SNEPPX_http_client_init(SNEPPXHttpClient* client, const char* base_url, int timeout_ms);
void SNEPPX_http_client_cleanup(SNEPPXHttpClient* client);
int SNEPPX_http_get(SNEPPXHttpClient* client, const char* path, SNEPPXHttpResponse* resp);
int SNEPPX_http_post(SNEPPXHttpClient* client, const char* path, const char* body, size_t body_len, const char* content_type, SNEPPXHttpResponse* resp);
int SNEPPX_http_put(SNEPPXHttpClient* client, const char* path, const char* body, size_t body_len, SNEPPXHttpResponse* resp);
int SNEPPX_http_delete(SNEPPXHttpClient* client, const char* path, SNEPPXHttpResponse* resp);
void SNEPPX_http_response_free(SNEPPXHttpResponse* resp);
#ifdef __cplusplus
}
#endif
#endif
