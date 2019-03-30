#ifndef _SOFTMIRROR_SERVER_WEBSERVER_H_
#define _SOFTMIRROR_SERVER_WEBSERVER_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include "globalConfig.h"

class Config;
class CoilMapper;

class WebServer {
private:
  char address[MAX_OSC_ADDRESS_LENGTH];
  char arg[MAX_OSC_STRING_ARG_LENGTH]; // for OSC string arguments
  char socketFrame[MAX_TOTAL_SOCKET_STRING_SIZE];
  // char coilFrames[MAX_CLIENT_BOARDS][MAX_SOCKET_COILFRAME_SIZE];
  int socketMsgLength;
  String socketMsg[MAX_TOTAL_SOCKET_MSG_LENGTH];
  int clientBoardsSocketIds[MAX_CLIENT_BOARDS];
  unsigned long lastUpdate;

  WiFiUDP udp;
  OSCMessage inputOSCMessage;
  ESP8266WebServer *webServer;
  WebSocketsServer socketServer = WebSocketsServer(81);

  Config *config;
  CoilMapper *mapper;
  
public:
  WebServer() {
    lastUpdate = millis();
    
    for (int i = 0; i < MAX_CLIENT_BOARDS; i++) {
      clientBoardsSocketIds[i] = -1;
    }
  }
  ~WebServer() {}

  void init(Config *c, CoilMapper *m);
  void start();
  void update();
  void processOSCMessage();
  void processSocketMessage(uint8_t num, uint8_t *payload, size_t length);
  void sendCoilMap(uint8_t num);
  void storeCoilMap();
  void sendCoilFrame(int boardIndex, int *pwmValues);
};

#endif /* _SOFTMIRROR_SERVER_WEBSERVER_H_ */
