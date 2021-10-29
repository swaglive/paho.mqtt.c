#include "MQTTTopicHelper.h"

#include <stdio.h>
#include <pthread.h>
#include "MQTTAsync.h"
#include "HelperUtils.h"

pthread_mutex_t sub_mutex;
pthread_cond_t sub_cv;

int subscribing = 0;
MQTTAsync_failureData5 *sub_failure;

void onSubscribe(void *context, MQTTAsync_successData5 *response);
void onSubscribeFailure(void *context, MQTTAsync_failureData5 *response);
int waitForSubscriptionResult(void);

__attribute__((visibility("default"))) __attribute__((used)) int MQTTHelper_subscribeMany(void *context, int count, char *const *topics, const MQTTQoS *qoslist)
{
  for (int i = 0; i < count; i++)
  {
    printf("[MQTT] Will subscribe to %s, QoS %d\n", topics[i], qoslist[i]);
  }
  if (!context)
  {
    printf("[MQTT] subscribe without context\n");
    return -1;
  }  
  MQTTAsync client = (MQTTAsync)context;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  int rc;

  opts.onSuccess5 = onSubscribe;
  opts.onFailure5 = onSubscribeFailure;
  opts.context = client;
  if ((rc = MQTTAsync_subscribeMany(client, count, topics, (const int *)qoslist, &opts)) != MQTTASYNC_SUCCESS)
  {
    printf("[MQTT] Failed to start subscribe, return code %d\n", rc);
    goto exit;
  }
  subscribing = 1;
  pthread_mutex_init(&sub_mutex, NULL);
  pthread_cond_init(&sub_cv, NULL);

  rc = waitForSubscriptionResult();

  destroyFailureData((void **)&sub_failure);
  pthread_mutex_destroy(&sub_mutex);
  pthread_cond_destroy(&sub_cv);
exit:
  return rc;
}

__attribute__((visibility("default"))) __attribute__((used)) int MQTTHelper_subscribe(void *context, const char *topic, MQTTQoS qos)
{
  return MQTTHelper_subscribeMany(context, (char *const *)&topic, 1, &qos);
}

int waitForSubscriptionResult()
{
  int rc = 0;
  pthread_mutex_lock(&sub_mutex);
  while (subscribing)
  {
    pthread_cond_wait(&sub_cv, &sub_mutex);
  }
  if (sub_failure)
  {
    rc = sub_failure->code;
  }
  pthread_mutex_unlock(&sub_mutex);

  return rc;
}

void onSubscribe(void *context, MQTTAsync_successData5 *response)
{
  pthread_mutex_lock(&sub_mutex);
  printf("[MQTT] Subscribe succeeded\n");
  subscribing = 0;
  pthread_cond_signal(&sub_cv);
  pthread_mutex_unlock(&sub_mutex);
}

void onSubscribeFailure(void *context, MQTTAsync_failureData5 *response)
{
  MQTTAsync_failureData5 *copy = copyFailureData(response);
  pthread_mutex_lock(&sub_mutex);
  printf("[MQTT] Subscribe failed, rc %d\n", response->code);
  sub_failure = copy;
  subscribing = 0;
  pthread_cond_signal(&sub_cv);
  pthread_mutex_unlock(&sub_mutex);
}
