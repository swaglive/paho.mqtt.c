#include "MQTTConnectionHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../MQTTAsync.h"
#include "HelperUtils.h"

typedef struct
{
  char *topic;
  MQTTAsync_message *message;
} RemoteMessage;

pthread_mutex_t msg_q_mutex;
pthread_cond_t nonempty_msg_q_cv;
RemoteMessage *last_message;
MQTTAsync_failureData *conn_failure;
ConnectionStatus conn_status = idle;

int onMessage(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
void onConnect(void *context, MQTTAsync_successData *response);
void onConnectFailure(void *context, MQTTAsync_failureData *response);
void onConnectionLost(void *context, char *cause);
void waitingForMessages(void *context, MessageCallback msgCb);
void waitingForConnection(MQTTAsync client, ConnectionCallback connCb);
void _freeLastMessage(void);
void _freeConnMessage(void);
void processMessages(void *context, MessageCallback msgCb);

__attribute__((visibility("default"))) __attribute__((used)) int MQTTHelper_connect(const char *brokerUri, const char *clientId, MessageCallback msgCb, ConnectionCallback connCb)
{
  if (!brokerUri || !clientId)
  {
    return -1;
  }
  printf("Broker: %s, Client: %s \n", brokerUri, clientId);
  MQTTAsync client;
  MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;
  int rc;

  if ((rc = MQTTAsync_create(&client, brokerUri, clientId, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS)
  {
    rc = EXIT_FAILURE;
  }
  if ((rc = MQTTAsync_setCallbacks(client, client, onConnectionLost, onMessage, NULL)) != MQTTASYNC_SUCCESS)
  {
    rc = EXIT_FAILURE;
    goto client_destroy_exit;
  }

  pthread_mutex_init(&msg_q_mutex, NULL);
  pthread_cond_init(&nonempty_msg_q_cv, NULL);
  opts.keepAliveInterval = 20;
  opts.cleansession = 1;
  opts.onSuccess = onConnect;
  opts.onFailure = onConnectFailure;
  opts.context = client;
  if ((rc = MQTTAsync_connect(client, &opts)) != MQTTASYNC_SUCCESS)
  {
    rc = EXIT_FAILURE;
    goto msg_q_destroy_exit;
  }
  conn_status = connecting;
  waitingForConnection(client, connCb);
  waitingForMessages(client, msgCb);

msg_q_destroy_exit:
  _freeLastMessage();
  destroyFailureData((void **)&conn_failure);
  pthread_mutex_destroy(&msg_q_mutex);
  pthread_cond_destroy(&nonempty_msg_q_cv);
client_destroy_exit:
  MQTTAsync_destroy(&client);
exit:
  return rc;
}

int onMessage(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
  char *topicBuf = calloc(topicLen + 1, sizeof(char *));
  MQTTAsync_message *msgBuf = calloc(1, sizeof(MQTTAsync_message));
  RemoteMessage *msg = calloc(1, sizeof(RemoteMessage));
  topicBuf = (char *)memcpy(topicBuf, topicName, topicLen);
  *(topicBuf + topicLen * sizeof(char *)) = '\0';
  msgBuf = (MQTTAsync_message *)memcpy(msgBuf, message, sizeof(MQTTAsync_message));
  msg->message = msgBuf;
  msg->topic = topicBuf;
  pthread_mutex_lock(&msg_q_mutex);
  _freeLastMessage();
  last_message = msg;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
  return 1;
}

void waitingForConnection(MQTTAsync client, ConnectionCallback connCb)
{
  pthread_mutex_lock(&msg_q_mutex);
  while (conn_status == connecting)
  {
    pthread_cond_wait(&nonempty_msg_q_cv, &msg_q_mutex);
  }
  printf("[MQTT] update connection status %d\n", conn_status);
  if (conn_status == connected)
  {
    (*connCb)(client, 0, NULL);
  }
  else
  {
    if (conn_failure)
    {
      (*connCb)(NULL, conn_failure->code, (char *)conn_failure->message);
      destroyFailureData((void **)&conn_failure);
    }
    else
    {
      (*connCb)(NULL, 1, NULL);
    }
  }
  pthread_mutex_unlock(&msg_q_mutex);
}

void onConnect(void *context, MQTTAsync_successData *response)
{
  pthread_mutex_lock(&msg_q_mutex);
  conn_status = connected;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
}

void onConnectionLost(void *context, char *cause)
{
  MQTTAsync_failureData response = {};
  response.code = 1;
  response.token = 0;
  response.message = cause;
  MQTTAsync_failureData *copy = copyFailureData(&response);
  pthread_mutex_lock(&msg_q_mutex);
  conn_failure = copy;
  conn_status = idle;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
}

void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
  MQTTAsync_failureData *copy = copyFailureData(response);
  pthread_mutex_lock(&msg_q_mutex);
  destroyFailureData((void **)&conn_failure);
  conn_failure = copy;
  conn_status = idle;
  pthread_cond_signal(&nonempty_msg_q_cv);
  pthread_mutex_unlock(&msg_q_mutex);
}

void waitingForMessages(void *context, MessageCallback msgCb)
{
  pthread_mutex_lock(&msg_q_mutex);
  while (conn_status == connected)
  {
    pthread_cond_wait(&nonempty_msg_q_cv, &msg_q_mutex);
    processMessages(context, msgCb);
  }
  pthread_mutex_unlock(&msg_q_mutex);
}

void processMessages(void *context, MessageCallback msgCb)
{
  if (!last_message)
  {
    return;
  }
  MQTTAsync_message *msgPtr = last_message->message;
  char *topic = last_message->topic;
  void *payload = msgPtr->payload;
  int payloadlen = msgPtr->payloadlen;
  (*msgCb)(context, topic, payload, payloadlen);
  _freeLastMessage();
}

void _freeLastMessage()
{
  if (last_message)
  {
    free(last_message->topic);
    free(last_message->message);
    free(last_message);
    last_message = NULL;
  }
}
