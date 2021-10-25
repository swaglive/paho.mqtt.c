typedef enum
{
  atMostOnce = 0,
  atLeastOnce = 1,
  exactlyOnce = 2,
} MQTTQoS;

int MQTTHelper_subscribeMany(void *client, char *const *topics, int count, MQTTQoS qos);
int MQTTHelper_unsubscribeMany(void *context, char *const *topics, int count);