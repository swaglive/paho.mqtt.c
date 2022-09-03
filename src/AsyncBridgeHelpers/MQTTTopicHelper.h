#ifndef MQTTTopicHelper_h
#define MQTTTopicHelper_h

typedef enum
{
  atMostOnce = 0,
  atLeastOnce = 1,
  exactlyOnce = 2,
} MQTTQoS;

int MQTTHelper_subscribe(void *context, const char *topic, MQTTQoS qos);
int MQTTHelper_subscribeMany(void *context, int count, char *const *topics, const MQTTQoS *qoslist);
int MQTTHelper_unsubscribe(void *context, const char *topic);
int MQTTHelper_unsubscribeMany(void *context, int count, char *const *topics);

#endif