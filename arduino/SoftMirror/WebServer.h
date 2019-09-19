#ifndef _SOFTMIRROR_WEBSERVER_H_
#define _SOFTMIRROR_WEBSERVER_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "globals.h"

class Config;
class CoilMapper;

class WebServer {
private:
  char address[MAX_OSC_ADDRESS_LENGTH];
  char arg[MAX_OSC_STRING_ARG_LENGTH]; // for OSC string arguments
  char socketFrame[MAX_TOTAL_SOCKET_STRING_SIZE];
  int socketMsgLength;
  String socketMsg[MAX_TOTAL_SOCKET_MSG_LENGTH];
  unsigned long lastUpdate;

  WiFiUDP udp;
  OSCMessage inputOSCMessage;
  ESP8266WebServer *webServer;
  WebSocketsServer socketServer = WebSocketsServer(81);

  Config *config;
  CoilMapper *mapper;
  
public:
  WebServer() { lastUpdate = millis(); }
  ~WebServer() {}

  void init(Config *c, CoilMapper *m);
  void start();
  void update();
  void processOSCMessage();
  void processSocketMessage(uint8_t num, uint8_t *payload, size_t length);
  void sendCoilMap(uint8_t num);
  void storeCoilMap();
};

#endif /* _SOFTMIRROR_WEBSERVER_H_ */
