#include "MQTTConnectionHelper.h"

#include <pthread.h>
#include <stdio.h>
#include "MQTTAsync.h"

extern pthread_mutex_t msg_q_mutex;
extern ConnectionStatus conn_status;
extern pthread_cond_t nonempty_msg_q_cv;

void onDisconnectFailure(void *context, MQTTAsync_failureData5 *response);
void onDisconnect(void *context, MQTTAsync_successData5 *response);

__attribute__((visibility("default"))) __attribute__((used)) int MQTTHelper_disconnect(void *context)
{
  if (!context)
  {
    return -1;
  }
  MQTTAsync client = (MQTTAsync)context;
  MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer5;
  opts.onSuccess5 = onDisconnect;
  opts.onFailure5 = onDisconnectFailure;
  return MQTTAsync_disconnect(client, &opts);
}

void onDisconnect(void *context, MQTTAsync_successData5 *response)
{
  printf("Successful disconnection\n");
  pthread_mutex_lock(&msg_q_mutex);
  conn_status = idle;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
}

void onDisconnectFailure(void *context, MQTTAsync_failureData5 *response)
{
  printf("Disconnect failed, rc %d\n", response->code);
  pthread_mutex_lock(&msg_q_mutex);
  conn_status = idle;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
}