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
  // yield();
  delay(1);
}

// #include "PWMInterface.h"

// PWMInterface pwm(2, 3);

// int brightness[6];
// int increment[6];

// void setup() {
//   // pwm.begin();
//   // pwm.setPWMFreq(1000);

//   // pwm2.begin();
//   // pwm2.setPWMFreq(1000);

//   // Wire.setClock(400000);
//   pwm.init();

//   for (int i = 0; i < 6; ++i) {
//     brightness[0] = 0;
//   }

//   increment[0] = 10;
//   increment[1] = 33;
//   increment[2] = 75;
//   increment[3] = 20;
//   increment[4] = 63;
//   increment[5] = 120;
// }

// void loop() {
//   for (int i = 0; i < 6; ++i) {
//     brightness[i] += increment[i];
//     if (brightness[i] >= 4096) {
//       brightness[i] -= 4096;
//     }

//     pwm.setPWM(i, brightness[i], 0);
//   }

//   // yield();
//   delay(10);
// }