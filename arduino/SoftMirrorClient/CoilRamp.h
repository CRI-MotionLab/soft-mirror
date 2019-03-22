#ifndef _SOFTMIRROR_CLIENT_COILRAMP_H_
#define _SOFTMIRROR_CLIENT_COILRAMP_H_

#include <Arduino.h>
#include "globalConfig.h"

class CoilRamp {
private:
  int pwmPin;
  int targetValue;
  float value;
  float increment;
  float incrementPeriods;
  bool ramping;

public:
  CoilRamp(int pin) :
  pwmPin(pin), value(0), increment(0), targetValue(0), ramping(false) {
    incrementPeriods = (MAIN_PERIOD < PWM_CONTROL_MSG_PERIOD)
                     ? (float) PWM_CONTROL_MSG_PERIOD / MAIN_PERIOD
                     : 1;
  }
  
  ~CoilRamp() {}

  void setTargetPwmValue(int newTargetValue) {
    if (targetValue != newTargetValue) {
      targetValue = newTargetValue;
      increment = (targetValue - value) / incrementPeriods;
      ramping = true;
    }
  }
  
  void update() {
    if (ramping) {    
      float nextValue = value + increment;
      
      if ((value < targetValue && nextValue >= targetValue) ||
          (value > targetValue && nextValue <= targetValue)) {
        value = targetValue;
        ramping = false;
      } else {
        value = nextValue;
      }
    }

    analogWrite(pwmPin, (int) value);
  }
};

#endif /* _SOFTMIRROR_CLIENT_COILRAMP_H_ */

