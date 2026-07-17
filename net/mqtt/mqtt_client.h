#ifndef SNEPPX_MQTT_CLIENT_H
#define SNEPPX_MQTT_CLIENT_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void* SNEPPX_mqtt_connect(const char* broker, int port, const char* client_id, int keepalive);
void SNEPPX_mqtt_disconnect(void* client);
int SNEPPX_mqtt_subscribe(void* client, const char* topic, int qos);
int SNEPPX_mqtt_unsubscribe(void* client, const char* topic);
int SNEPPX_mqtt_publish(void* client, const char* topic, const unsigned char* payload, size_t len, int qos, int retain);
int SNEPPX_mqtt_set_on_message(void* client, void (*cb)(void* client, const char* topic, const unsigned char* payload, size_t len));
int SNEPPX_mqtt_loop_start(void* client);
int SNEPPX_mqtt_loop_stop(void* client);
int SNEPPX_mqtt_is_connected(void* client);
#ifdef __cplusplus
}
#endif
#endif
