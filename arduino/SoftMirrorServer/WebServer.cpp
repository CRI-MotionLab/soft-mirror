#include "Config.h"
#include "CoilMapper.h"
#include "configWebPage.h"
#include "WebServer.h"

void
WebServer::init(Config *c, CoilMapper *m) {
  config = c;
  mapper = m;
  webServer = new ESP8266WebServer(80);
}

void
WebServer::start() {
  WiFi.mode(WIFI_AP);
  IPAddress ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(ip, gateway, subnet);

  // if (WiFi.softAP("softmirror")) {
  if (WiFi.softAP("softmirror", "mirrorsoft", 1, false, 8)) {
    digitalWrite(NODE_LED, LOW);
    
    udp.begin(8000); // start listening to udp messages

    webServer->on("/", [&]() {
      webServer->send(200, "text/html", configWebPage);
    });
    webServer->begin();

    socketServer.begin();    
    socketServer.onEvent([&](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
      switch (type) {
        case WStype_DISCONNECTED:
        // num is socket id
        for (int i = 0; i < MAX_CLIENT_BOARDS; i++) {
          int socketId = clientBoardsSocketIds[i];
          if (i != -1 && (uint8_t) socketId == num) {
            clientBoardsSocketIds[i] = -1;
          }
        }
        break;
      case WStype_CONNECTED: {
          // IPAddress ip = socketServer.remoteIP(num);
          // payload is url
          // socketServer.sendTXT(num, "Connected");
#ifdef USE_SERIAL
          Serial.printf("new board connected : %i\n", (int) num);
#endif /* USE_SERIAL */
        }
        break;
      case WStype_TEXT:
        processSocketMessage(num, payload, length);
        break;
      case WStype_BIN:
        // hexdump(payload, length);
        // socketServer.sendBIN(num, payload, length);
        break;
      }
    });

#ifdef USE_SERIAL
    Serial.println("server initialized");
#endif /* USE_SERIAL */
  }
}

void
WebServer::update() {
  unsigned long now = millis();
  if (now - lastUpdate < MAIN_PERIOD) return;
  lastUpdate = now;
  
  int packetSize = udp.parsePacket();
  
  if (packetSize > 0) {
    // read incoming message into the bundle
    while (packetSize--) { inputOSCMessage.fill(udp.read()); }
    if (!inputOSCMessage.hasError()) { processOSCMessage(); }
    else { Serial.println("bad osc message"); }
    inputOSCMessage.empty();
  }

  socketServer.loop();
  webServer->handleClient();
  mapper->update();
}

void
WebServer::processOSCMessage() {
  int msgLength = inputOSCMessage.size();
  inputOSCMessage.getAddress(address);

#ifdef USE_SERIAL
  // Serial.printf("received message %s with %i arguments\n", address, msgLength);
#endif /* USE_SERIAL */
  
  if (strcmp(address, "/centroid") == 0 && msgLength > 1 &&
    inputOSCMessage.getType(0) == 'f' && inputOSCMessage.getType(1) == 'f') {
    float x = inputOSCMessage.getFloat(0);
    float y = inputOSCMessage.getFloat(1);
    // compute centroid velocity here ?
    // then call : mapper->setCentroid(CLIP(x, 0, 1), CLIP(y, 0, 1), velocity) ?
    mapper->setCentroid(CLIP(x, 0, 1), CLIP(y, 0, 1));
  } else if (strcmp(address, "/automatic") == 0 && msgLength > 0) {
    if (inputOSCMessage.getInt(0) == 1) {
      mapper->setMode(ControlModeAutomatic);
    } else {
      mapper->setMode(ControlModeGesture);
    }
  }
}

////////// from coil map web editor :

void
WebServer::processSocketMessage(uint8_t num, uint8_t *payload, size_t length) {
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

  // socketMsgLength++;

#ifdef USE_SERIAL
  Serial.printf(
    "received socket message : %s of length %i\n",
    socketMsg[0].c_str(), socketMsgLength
  );
#endif /* USE_SERIAL */

  if (socketMsg[0] == "getcoilmap") {
    sendCoilMap(num);
  } else if (socketMsgLength >= 6 &&
             socketMsg[0] == "coilmap") {
    storeCoilMap();
  } else if (socketMsgLength >= 2 &&
             socketMsg[0] == "boardid") {
    clientBoardsSocketIds[socketMsg[1].toInt() - 1] = (int) num;
#ifdef USE_SERIAL
  Serial.printf("new board connected : %i (socket id : %i)\n", socketMsg[1].toInt(), (int) num);
#endif /* USE_SERIAL */
  }
}

void
WebServer::sendCoilMap(uint8_t num) {
  config->writeCoilMapString(socketFrame);  
#ifdef USE_SERIAL
  Serial.printf("sending coil map:\n%s", socketFrame);
#endif /* USE_SERIAL */
  socketServer.sendTXT(num, socketFrame);
}

void
WebServer::storeCoilMap() {
  if (socketMsg[1] == "maxdims") {
    config->setMaxDims(socketMsg[2].toInt(), socketMsg[3].toInt());
  }

  int n = 0;
  
  if (socketMsg[4] == "startmap") {
    int i = 5;
    while (socketMsg[i] != "endmap" && n < MAX_NUMBER_OF_COILS) {
      
#ifdef USE_SERIAL
  Serial.printf(
    "storing coil %i : %i %i\n",
    socketMsg[i + 2].toInt(), socketMsg[i].toInt(), socketMsg[i + 1].toInt()
   );
#endif /* USE_SERIAL */

      config->setCoil(socketMsg[i + 2].toInt(), socketMsg[i].toInt(), socketMsg[i + 1].toInt());
      i += 3;
      n++;
    }
  }

  config->setNbCoils(n);
  config->storeCoils();
}

void
WebServer::sendCoilFrame(int boardIndex, int *pwmValues) {
  if (clientBoardsSocketIds[boardIndex - 1] == -1) return;

  strcpy(socketFrame, "coilframe\n");
  // strcpy(coilFrames[boardIndex - 1], "coilframe\n");
  
  String tmp = String(boardIndex) + "\n";

  for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
    tmp = String(pwmValues[i]) + "\n";
    strcat(socketFrame, tmp.c_str());
    // strcat(coilFrames[boardIndex - 1], tmp.c_str());
  }

#ifdef USE_SERIAL
  Serial.printf("sending coil frame :\n%s", socketFrame);
#endif /* USE_SERIAL */

  socketServer.sendTXT((uint8_t) clientBoardsSocketIds[boardIndex - 1], socketFrame);
  // socketServer.broadcastTXT(socketFrame);
}

