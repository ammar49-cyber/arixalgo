#include "http_server.h"
#include <stdlib.h>
#include <string.h>

int SNEPPX_http_server_start(int port, int num_threads) { (void)port; (void)num_threads; return 0; }
void SNEPPX_http_server_stop(void) {}
int SNEPPX_http_server_register_route(const char* method, const char* path, void* handler) { (void)method; (void)path; (void)handler; return 0; }
int SNEPPX_http_server_register_static_dir(const char* url_prefix, const char* dir_path) { (void)url_prefix; (void)dir_path; return 0; }
int SNEPPX_http_server_register_middleware(void* middleware_fn) { (void)middleware_fn; return 0; }
void* SNEPPX_http_request_get_header(void* req, const char* name) { (void)req; (void)name; return NULL; }
const char* SNEPPX_http_request_get_body(void* req, size_t* len) { (void)req; if (len) *len = 0; return NULL; }
int SNEPPX_http_response_set_status(void* resp, int status_code) { (void)resp; (void)status_code; return 0; }
int SNEPPX_http_response_set_header(void* resp, const char* name, const char* value) { (void)resp; (void)name; (void)value; return 0; }
int SNEPPX_http_response_set_body(void* resp, const char* data, size_t len) { (void)resp; (void)data; (void)len; return 0; }
int SNEPPX_http_response_send_json(void* resp, const char* json) { (void)resp; (void)json; return 0; }
