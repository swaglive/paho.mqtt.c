typedef void (*ConnectionCallback)(void *client, int code, const char *messages);
typedef void (*MessageCallback)(const char *topic, const void *payload, int payloadlen);
typedef enum
{
  idle = 0,
  connecting = 1,
  connected = 2
} ConnectionStatus;

int MQTTHelper_connect(const char *brokerUri, const char *clientId, MessageCallback msgCb, ConnectionCallback connCb);
int MQTTHelper_disconnect(void *context);
