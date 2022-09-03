#ifndef MQTTUserProperty_h
#define MQTTUserProperty_h

typedef struct
{
  char *key;
  char *value;
} MQTTHelper_UserProperty;

typedef struct
{
  int len;
  MQTTHelper_UserProperty *data;
} MQTTHelper_UserProperties;

#endif