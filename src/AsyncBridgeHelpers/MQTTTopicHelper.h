typedef enum
{
  atMostOnce = 0,
  atLeastOnce = 1,
  exactlyOnce = 2,
} MQTTQoS;

int MQTTHelper_subscribe(void *context, const char *topic, MQTTQoS qos);
int MQTTHelper_subscribeMany(void *context, char *const *topics, int count, MQTTQoS qos);
int MQTTHelper_unsubscribe(void *context, const char *topic);
int MQTTHelper_unsubscribeMany(void *context, char *const *topics, int count);