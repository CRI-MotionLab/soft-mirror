#include "globals.h"
#include "Config.h"
#include "CoilMapper.h"
#include "WebServer.h"

Config config;
CoilMapper mapper;
WebServer server;

void setup() {
  pinMode(NODE_LED, OUTPUT);
  digitalWrite(NODE_LED, HIGH);

#ifdef USE_SERIAL
  Serial.begin(9600);
#endif /* USE_SERIAL */

  config.init(&mapper);
  mapper.init(&server);
  server.init(&config, &mapper);
  server.start();
}

void loop() {
  server.update();
  delay(1);
}
