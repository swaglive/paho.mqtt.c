#include "MQTTTopicHelper.h"

#include <stdio.h>
#include <pthread.h>
#include "../MQTTAsync.h"
#include "HelperUtils.h"

pthread_mutex_t unsub_mutex;
pthread_cond_t unsub_cv;

int unsubscribing = 0;
MQTTAsync_failureData *unsub_failure;

void onUnsubscribe(void *context, MQTTAsync_successData *response);
void onUnsubscribeFailure(void *context, MQTTAsync_failureData *response);
int waitForUnsubscriptionResult(void);

__attribute__((visibility("default"))) __attribute__((used)) int MQTTHelper_unsubscribeMany(void *context, int count, char *const *topics)
{
  if (!context)
  {
    return -1;
  }
  MQTTAsync client = (MQTTAsync)context;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

  int rc;

  opts.onSuccess = onUnsubscribe;
  opts.onFailure = onUnsubscribeFailure;
  opts.context = client;

  if ((rc = MQTTAsync_unsubscribeMany(client, count, topics, &opts)) != MQTTASYNC_SUCCESS)
  {
    printf("Failed to start subscribe, return code %d\n", rc);
    goto exit;
  }

  unsubscribing = 1;
  pthread_mutex_init(&unsub_mutex, NULL);
  pthread_cond_init(&unsub_cv, NULL);

  rc = waitForUnsubscriptionResult();

  destroyFailureData((void **)&unsub_failure);
  pthread_mutex_destroy(&unsub_mutex);
  pthread_cond_destroy(&unsub_cv);
exit:
  return rc;
}

__attribute__((visibility("default"))) __attribute__((used)) int MQTTHelper_unsubscribe(void *context, const char *topic)
{
  return MQTTHelper_unsubscribeMany(context, (char *const *)&topic, 1);
}

int waitForUnsubscriptionResult(void)
{
  int rc = 0;
  pthread_mutex_lock(&unsub_mutex);
  while (unsubscribing)
  {
    pthread_cond_wait(&unsub_cv, &unsub_mutex);
  }
  if (unsub_failure)
  {
    rc = unsub_failure->code;
  }
  pthread_mutex_unlock(&unsub_mutex);

  return rc;
}

void onUnsubscribe(void *context, MQTTAsync_successData *response)
{
  pthread_mutex_lock(&unsub_mutex);
  printf("Unsubscribe succeeded\n");
  unsubscribing = 0;
  pthread_cond_signal(&unsub_cv);
  pthread_mutex_unlock(&unsub_mutex);
}

void onUnsubscribeFailure(void *context, MQTTAsync_failureData *response)
{
  MQTTAsync_failureData *copy = copyFailureData(response);
  pthread_mutex_lock(&unsub_mutex);
  printf("Subscribe failed, rc %d\n", response->code);
  unsub_failure = copy;
  unsubscribing = 0;
  pthread_cond_signal(&unsub_cv);
  pthread_mutex_unlock(&unsub_mutex);
}