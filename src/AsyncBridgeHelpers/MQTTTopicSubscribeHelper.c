#include "MQTTTopicHelper.h"

#include <stdio.h>
#include <pthread.h>
#include "../MQTTAsync.h"
#include "HelperUtils.h"

pthread_mutex_t sub_mutex;
pthread_cond_t sub_cv;

int subscribing = 0;
MQTTAsync_failureData *sub_failure;

void onSubscribe(void *context, MQTTAsync_successData *response);
void onSubscribeFailure(void *context, MQTTAsync_failureData *response);
int waitForSubscriptionResult(void);

int MQTTHelper_subscribeMany(void *context, char *const *topics, int count, const MQTTQoS qos)
{
  MQTTAsync client = (MQTTAsync)context;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  int rc;

  opts.onSuccess = onSubscribe;
  opts.onFailure = onSubscribeFailure;
  opts.context = client;
  if ((rc = MQTTAsync_subscribeMany(client, count, topics, (const int *)&qos, &opts)) != MQTTASYNC_SUCCESS)
  {
    printf("Failed to start subscribe, return code %d\n", rc);
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

int MQTTHelper_subscribe(void *context, const char *topic, MQTTQoS qos)
{
  return MQTTHelper_subscribeMany(context, (char *const *)&topic, 1, qos);
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

void onSubscribe(void *context, MQTTAsync_successData *response)
{
  pthread_mutex_lock(&sub_mutex);
  printf("Subscribe succeeded\n");
  subscribing = 0;
  pthread_cond_signal(&sub_cv);
  pthread_mutex_unlock(&sub_mutex);
}

void onSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
  MQTTAsync_failureData *copy = copyFailureData(response);
  pthread_mutex_lock(&sub_mutex);
  printf("Subscribe failed, rc %d\n", response->code);
  sub_failure = copy;
  subscribing = 0;
  pthread_cond_signal(&sub_cv);
  pthread_mutex_unlock(&sub_mutex);
}
