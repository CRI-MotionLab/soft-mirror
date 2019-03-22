/**
 * SoftMirror Server Firmware for NodeMCU v1.0-12e
 * 
 * NB :
 * nodemcu's builtin led pin is 16 (<=> D0)
 * esp8266's builtin led pin is 2 (<=> D4)
 * pinout : { D0:16, D1:5, D2:4, D3:0, D4:2, D5:14, D6:12, D7:13, D8:15 }
 * don't use 16 in case ...
 * pinout suite : { RX:3, TX:1, SD2:9, SD3:10 } // might be used for pwm
 */

// !!!!
// The WebSockets library has been modified to run in async mode
// (commented line 89 and uncommented line 90 in WebSockets.h)
// this means the ESPAsyncRCP library (https://github.com/me-no-dev/ESPAsyncTCP) is required
// !!!!

#include "globalConfig.h"
#include "Config.h"
#include "CoilMapper.h"
#include "WebServer.h"

Config config;
CoilMapper mapper;
WebServer server;

void setup() {
  pinMode(NODE_LED, OUTPUT);
  digitalWrite(NODE_LED, HIGH);

  for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
    pinMode(PWM_PINS_ARRAY[i], OUTPUT);
    analogWrite(PWM_PINS_ARRAY[i], 0);
  }

  if (PWM_OUTPUTS_PER_NODEMCU < 9) { // set back RX TX mode
    pinMode(1, FUNCTION_0);
    pinMode(3, FUNCTION_0);
  }

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
