#ifndef _SOFTMIRROR_CLIENT_WEBCLIENT_H_
#define _SOFTMIRROR_CLIENT_WEBCLIENT_H_

#include <WebSocketsClient.h>
#include "CoilRamp.h"
#include "globalConfig.h"

class WebClient {
private:
  char socketFrame[MAX_TOTAL_SOCKET_STRING_SIZE];
  int socketMsgLength;
  String socketMsg[MAX_TOTAL_SOCKET_MSG_LENGTH];
  bool ledStatus = false;
  unsigned long lastUpdate;

  WebSocketsClient socketClient;

  // ramp classes
  CoilRamp *coilRamps[PWM_OUTPUTS_PER_NODEMCU];

public:
  WebClient() {
    lastUpdate = millis();

    for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
      coilRamps[i] = new CoilRamp(PWM_PINS_ARRAY[i]);
    }
  }
  
  ~WebClient() {
    for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
      delete coilRamps[i];
    }
  }

  void start();
  void update();
  void processSocketMessage(uint8_t *payload, size_t length);
};

#endif /* _SOFTMIRROR_CLIENT_WEBCLIENT_H_ */
