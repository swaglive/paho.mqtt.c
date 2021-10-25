#include "HelperUtils.h"

#include <string.h>
#include <stdlib.h>
#include "../MQTTAsync.h"

void *copyFailureData(void *failureData)
{
  if (!failureData)
  {
    return NULL;
  }
  MQTTAsync_failureData *data = (MQTTAsync_failureData *)failureData;
  MQTTAsync_failureData *copyBuf = (MQTTAsync_failureData *)calloc(1, sizeof(MQTTAsync_failureData));
  copyBuf->token = data->token;
  copyBuf->code = data->code;
  if (data->message)
  {
    size_t msgLen = strlen(data->message) + 1;
    char *msgBuf = calloc(msgLen, sizeof(char *));
    msgBuf = (char *)memcpy(msgBuf, data->message, msgLen);
    copyBuf->message = msgBuf;
  }
  else
  {
    copyBuf->message = NULL;
  }
  return copyBuf;
}

void destroyFailureData(void **failureDataPtr)
{
  if (!(*failureDataPtr))
  {
    return;
  }
  MQTTAsync_failureData *data = (MQTTAsync_failureData *)(*failureDataPtr);
  free((void *)data->message);
  free(data);
  *failureDataPtr = NULL;
}
