#ifndef HelperUtils_h
#define HelperUtils_h

#include "MQTTUserProperty.h"

void *copyFailureData(void *failureData);
void destroyFailureData(void **failureDataPtr);
MQTTHelper_UserProperties getUserPropertiesFromMessage(void *);
void freeUserProperties(MQTTHelper_UserProperties);

#endif