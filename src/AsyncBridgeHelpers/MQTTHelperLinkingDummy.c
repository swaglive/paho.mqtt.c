#include <stdlib.h>
#include "MQTTHelperLinkingDummy.h"
#include "MQTTConnectionHelper.h"
#include "MQTTTopicHelper.h"

__attribute__((visibility("default"))) __attribute__((used)) int MQTTHelper_linkingDummy()
{
  MQTTHelper_subscribe(NULL, NULL, atMostOnce);
  MQTTHelper_unsubscribe(NULL, NULL);
  MQTTHelper_disconnect(NULL);
  MQTTHelper_connect(NULL, NULL, NULL, NULL);
  return 0;
}