#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "WebClient.h"

void
WebClient::start() {
  String boardIdMsg("boardid\n");
  boardIdMsg += String(BOARD_ID);
  boardIdMsg += "\n";
  strcpy(socketFrame, boardIdMsg.c_str());
  
#ifdef USE_SERIAL
  Serial.printf("%s", socketFrame);
#endif /* USE_SERIAL */

  WiFi.mode(WIFI_STA);
  WiFi.begin("softmirror", "mirrorsoft");

  while (WiFi.status() != WL_CONNECTED) {
    ledStatus = !ledStatus;
    digitalWrite(NODE_LED, (ledStatus ? LOW : HIGH));
    delay(100);
  }

  ledStatus = true;
  digitalWrite(NODE_LED, LOW);

  socketClient.begin("192.168.1.1", 81, "/");
  
  // socketClient.onEvent(webSocketEvent);
  socketClient.onEvent([&](WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
      case WStype_DISCONNECTED:
        break;
      case WStype_CONNECTED: {
          // payload is url
          // socketClient.sendTXT("Connected");          
          socketClient.sendTXT(socketFrame);
        }
        break;
      case WStype_TEXT:
        processSocketMessage(payload, length);
        break;
      case WStype_BIN:
        // hexdump(payload, length);
        // socketClient.sendBIN(payload, length);
        break;
    }
  });
  
  // use HTTP Basic Authorization this is optional remove if not needed
  // socketClient.setAuthorization("user", "Password");
  socketClient.setReconnectInterval(5000);
}

void
WebClient::update() {
  unsigned long now = millis();
  if (now - lastUpdate < MAIN_PERIOD) return;
  lastUpdate = now;
  
  socketClient.loop();
  for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
    coilRamps[i]->update();
  }
}

void
WebClient::processSocketMessage(uint8_t *payload, size_t length) {
  socketMsgLength = 0;
  socketMsg[0] = String("");

  for (size_t index = 0; index < length; index++) {
    if (payload[index] != '\n') {
      socketMsg[socketMsgLength] += (char) payload[index];
    } else {
      if (socketMsgLength == MAX_TOTAL_SOCKET_MSG_LENGTH) break;
      socketMsgLength++;
      socketMsg[socketMsgLength] = String("");
    }
  }

#ifdef USE_SERIAL
  for (int i = 0; i < socketMsgLength; i++) {
    Serial.printf("%s\n", socketMsg[i].c_str());
  }
#endif /* USE_SERIAL */

  if (socketMsg[0] == "coilframe") {
    for (int i = 0; i < socketMsgLength - 1; i++) {
      if (i < PWM_OUTPUTS_PER_NODEMCU) {
        coilRamps[i]->setTargetPwmValue(socketMsg[i + 1].toInt());
      } else {
        break;
      }
    }
  }
}

