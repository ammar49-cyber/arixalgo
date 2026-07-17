#include "mqtt_client.h"
#include <stdlib.h>
#include <string.h>

void* SNEPPX_mqtt_connect(const char* broker, int port, const char* client_id, int keepalive) { (void)broker; (void)port; (void)client_id; (void)keepalive; return NULL; }
void SNEPPX_mqtt_disconnect(void* client) { free(client); }
int SNEPPX_mqtt_subscribe(void* client, const char* topic, int qos) { (void)client; (void)topic; (void)qos; return 0; }
int SNEPPX_mqtt_unsubscribe(void* client, const char* topic) { (void)client; (void)topic; return 0; }
int SNEPPX_mqtt_publish(void* client, const char* topic, const unsigned char* payload, size_t len, int qos, int retain) { (void)client; (void)topic; (void)payload; (void)len; (void)qos; (void)retain; return 0; }
int SNEPPX_mqtt_set_on_message(void* client, void (*cb)(void* client, const char* topic, const unsigned char* payload, size_t len)) { (void)client; (void)cb; return 0; }
int SNEPPX_mqtt_loop_start(void* client) { (void)client; return 0; }
int SNEPPX_mqtt_loop_stop(void* client) { (void)client; return 0; }
int SNEPPX_mqtt_is_connected(void* client) { (void)client; return 0; }
