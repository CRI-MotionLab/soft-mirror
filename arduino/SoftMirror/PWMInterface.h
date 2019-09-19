#ifndef _SOFTMIRROR_PWMINTERFACE_H_
#define _SOFTMIRROR_PWMINTERFACE_H_

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "globals.h"

class PWMInterface {
private:
  //////////////////// PRIVATE RAMP CLASS

  class Ramp {
  private:
    int16_t targetValue;
    float value;
    // float targetDate;
    // float lastUpdateDate;
    float increment;
    float incrementPeriods;
    bool ramping;

  public:
    Ramp(float incPeriods) :
    targetValue(0),
    value(0),
    // targetDate(0),
    // lastUpdateDate(0),
    increment(0),
    incrementPeriods(incPeriods),
    ramping(false) {}

    ~Ramp() {}

    void setTargetValue(int16_t newTargetValue) {
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
    }

    /*
    void setTargetValue(uint16_t newTargetValue, float duration = 0) {
      if (targetValue != newTargetValue) {
        float now = static_cast<float>(millis());

        targetValue = newTargetValue;
        targetDate = now + duration;
        lastUpdateDate = now;        
        ramping = true;
      }
    }

    void update() {
      if (ramping) {
        float now = static_cast<float>(millis());
  
        if (now >= targetDate) {
          value = targetValue;
          ramping = false;
        }

        float nextValue = value +
          (targetValue - value) *
          (now - lastUpdateDate) /
          (targetDate - lastUpdateDate);
        
        if ((value < targetValue && nextValue >= targetValue) ||
            (value > targetValue && nextValue <= targetValue)) {
          value = targetValue;
          ramping = false;
        } else {
          value = nextValue;
          lastUpdateDate = now;
        }
      }
    }
    //*/

    int16_t getValue() {
      return static_cast<int16_t>(value);
    }
  };

  //////////////////// OTHER VARIABLES

  uint16_t nbOutputs;
  uint8_t nbCards;
  uint8_t nbOutputsPerCard;

  Ramp *ramps[MAX_NUMBER_OF_COILS];
  Adafruit_PWMServoDriver *pwms[PWM_BOARDS];

public:
  PWMInterface() {
    nbOutputs = 0;

    float incrementPeriods = MAIN_PERIOD < PWM_CONTROL_MSG_PERIOD
                           ? (float) PWM_CONTROL_MSG_PERIOD / MAIN_PERIOD
                           : 1;


    for (uint16_t i = 0; i < MAX_NUMBER_OF_COILS; ++i) {
      ramps[i] = new Ramp(incrementPeriods);
    }

    int address = 0x40;

    for (uint8_t i = 0; i < PWM_BOARDS; ++i) {
      pwms[i] = new Adafruit_PWMServoDriver(address);
      address++;
    }
  }

  ~PWMInterface() {
    for (uint16_t i = 0; i < MAX_NUMBER_OF_COILS; ++i) {
      delete ramps[i];
    }

    for (uint8_t i = 0; i < PWM_BOARDS; ++i) {
      delete pwms[i];
    }
  }

  void init(float pwmFreq = 1000, bool fastWire = true) {
    for (uint8_t i = 0; i < PWM_BOARDS; ++i) {
      pwms[i]->begin();
      pwms[i]->setPWMFreq(pwmFreq);
    }

    if (fastWire) {
      Wire.setClock(400000);
    }
  }

  void update() {
    for (uint16_t i = 0; i < nbOutputs; ++i) {
      ramps[i]->update();

      uint8_t board = i / OUTPUTS_PER_PWM_BOARD;
      uint8_t localOutput = i % OUTPUTS_PER_PWM_BOARD;
      pwms[board]->setPin(localOutput, ramps[i]->getValue(), INVERT_PWM);
      // pwms[board]->setPWM(localOutput, 0, ramps[i]->getValue());
    }

#ifdef USE_SERIAL
    // Serial.printf("%u\n", ramps[0]->getValue());
#endif /* USE_SERIAL */
  }

  void setNbCoils(uint16_t n) {
    nbOutputs = n;
  }

  void setTargetValue(uint8_t output, int16_t targetValue) {
    ramps[output]->setTargetValue(targetValue);
  }
};

#endif /* _SOFTMIRROR_PWMINTERFACE_H_ */
