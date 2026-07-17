#ifndef SNEPPX_HTTP_SERVER_H
#define SNEPPX_HTTP_SERVER_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int SNEPPX_http_server_start(int port, int num_threads);
void SNEPPX_http_server_stop(void);
int SNEPPX_http_server_register_route(const char* method, const char* path, void* handler);
int SNEPPX_http_server_register_static_dir(const char* url_prefix, const char* dir_path);
int SNEPPX_http_server_register_middleware(void* middleware_fn);
void* SNEPPX_http_request_get_header(void* req, const char* name);
const char* SNEPPX_http_request_get_body(void* req, size_t* len);
int SNEPPX_http_response_set_status(void* resp, int status_code);
int SNEPPX_http_response_set_header(void* resp, const char* name, const char* value);
int SNEPPX_http_response_set_body(void* resp, const char* data, size_t len);
int SNEPPX_http_response_send_json(void* resp, const char* json);
#ifdef __cplusplus
}
#endif
#endif
