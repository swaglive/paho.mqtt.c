#include "MQTTConnectionHelper.h"

#include <pthread.h>
#include <stdio.h>
#include "../MQTTAsync.h"

extern pthread_mutex_t msg_q_mutex;
extern ConnectionStatus conn_status;
extern pthread_cond_t nonempty_msg_q_cv;

void onDisconnectFailure(void *context, MQTTAsync_failureData *response);
void onDisconnect(void *context, MQTTAsync_successData *response);

int MQTTHelper_disconnect(void *context)
{
  MQTTAsync client = (MQTTAsync)context;
  MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
  opts.onSuccess = onDisconnect;
  opts.onFailure = onDisconnectFailure;
  return MQTTAsync_disconnect(client, &opts);
}

void onDisconnect(void *context, MQTTAsync_successData *response) {
  printf("Successful disconnection\n");
  pthread_mutex_lock(&msg_q_mutex);
  conn_status = idle;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
}

void onDisconnectFailure(void *context, MQTTAsync_failureData *response) {
  printf("Disconnect failed, rc %d\n", response->code);
  pthread_mutex_lock(&msg_q_mutex);
  conn_status = idle;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
}