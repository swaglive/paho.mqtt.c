#include "HelperUtils.h"

#include <string.h>
#include "Heap.h"
#include "MQTTAsync.h"

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

static char *copyLenString(MQTTLenString);

MQTTHelper_UserProperties getUserPropertiesFromMessage(void *messagePtr)
{
  MQTTHelper_UserProperties retVal = {0, NULL};
  MQTTAsync_message *message = (MQTTAsync_message *)messagePtr;
  if (!MQTTProperties_hasProperty(&(message->properties), MQTTPROPERTY_CODE_USER_PROPERTY))
  {
    printf("[MQTT] Message doesn't have user properties\n");
    return retVal;
  }
  int count = MQTTProperties_propertyCount(&(message->properties), MQTTPROPERTY_CODE_USER_PROPERTY);
  printf("[MQTT] Message has %d properties", count);
  MQTTHelper_UserProperty *properties = (MQTTHelper_UserProperty *)calloc(1, count * sizeof(MQTTHelper_UserProperty));
  for (int i = 0; i < count; i++)
  {
    MQTTHelper_UserProperty userProp = properties[i];
    MQTTProperty *property = MQTTProperties_getPropertyAt(&(message->properties), MQTTPROPERTY_CODE_USER_PROPERTY, i);
    userProp.key = copyLenString(property->value.data);
    userProp.value = copyLenString(property->value.value);
  }
  retVal.len = count;
  retVal.data = properties;
  return retVal;
}

static char *copyLenString(MQTTLenString lenString)
{
  char *buf = (char *)calloc(lenString.len + 1, sizeof(char));
  buf = (char *)memcpy(buf, lenString.data, lenString.len);
  buf[lenString.len] = '\0';
  return buf;
}

void freeUserProperties(MQTTHelper_UserProperties props)
{
  for (int i = 0; i < props.len; i++)
  {
    free(props.data[i].key);
    free(props.data[i].value);
  }
  free(props.data);
}
