#include "globalConfig.h"
#include "CoilRamp.h"

int coilCnt = 0;
unsigned long lastTick = 0;
unsigned long lastRampDate = 0;
CoilRamp *coilRamps[PWM_OUTPUTS_PER_NODEMCU];


void setup() {
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  
  analogWriteFreq(500);
  
  for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
    pinMode(PWM_PINS_ARRAY[i], OUTPUT);
    analogWrite(PWM_PINS_ARRAY[i], 0);
  }

  if (PWM_OUTPUTS_PER_NODEMCU < 9) { // set back RX TX mode
    pinMode(1, FUNCTION_0);
    pinMode(3, FUNCTION_0);
  }

  lastRampDate = lastTick = millis();
  for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
    coilRamps[i] = new CoilRamp(PWM_PINS_ARRAY[i]);
  }

  delay(1000);

  coilRamps[0]->setTargetPwmValue(MAX_PWM_VALUE);
}

void loop() {
  unsigned long now = millis();

  if (now - lastRampDate >= PWM_CONTROL_MSG_PERIOD) {
    lastRampDate = now;
    int coilId = coilCnt / 2;
    int coilState = coilCnt % 2;
    
    if (coilState == 0) {
      analogWriteFreq(random(500, 1000));
      int prevCoilId = (coilId == 0) ? (PWM_OUTPUTS_PER_NODEMCU - 1) : (coilId - 1);
      coilRamps[prevCoilId]->setValue(0);
      coilRamps[coilId]->setTargetPwmValue(MAX_PWM_VALUE);
    } else {
      coilRamps[coilId]->setTargetPwmValue(MIN_PWM_VALUE);
    }

    coilCnt = (coilCnt + 1) % (2 * PWM_OUTPUTS_PER_NODEMCU);
  }
  
  if (now - lastTick >= MAIN_PERIOD) {
    lastTick = now;
    for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
      coilRamps[i]->update();
    }
  }

  delay(1);
}

