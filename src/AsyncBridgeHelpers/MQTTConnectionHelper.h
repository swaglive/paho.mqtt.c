#ifndef MQTTConnectionHelper_h
#define MQTTConnectionHelper_h

#include "MQTTUserProperty.h"

typedef void (*ConnectionCallback)(void *client, int code, const char *messages);
typedef void (*MessageCallback)(void *client, const char *topic, const void *payload, int payloadlen, MQTTHelper_UserProperties props);
typedef enum
{
  idle = 0,
  connecting = 1,
  connected = 2
} ConnectionStatus;

//TODO: add auth
int MQTTHelper_connect(const char *brokerUri, const char *clientId, MessageCallback msgCb, ConnectionCallback connCb);
int MQTTHelper_connect(const char *brokerUri, const char *clientId, MessageCallback msgCb, ConnectionCallback connCb, const char *CAfile, const char *CApath);
int MQTTHelper_disconnect(void *context);

#endif