#ifndef _SOFTMIRROR_COILMAPPER_H_
#define _SOFTMIRROR_COILMAPPER_H_

// #include "CoilRamp.h"
#include "PWMInterface.h"
#include "globals.h"

class WebServer;

class CoilMapper {
private:
  int nbCoils;
  float coils[MAX_NUMBER_OF_COILS][2];

  // variables to compute closest neightbors and their distances
  int closestIndexes[MAX_NUMBER_OF_CLOSEST_COILS];
  int nbValidDistances;
  int nbClosest;
  float distances[MAX_NUMBER_OF_COILS];
  int distanceIndexes[MAX_NUMBER_OF_COILS];

  // variables to compute centroid stuff
  bool incomingCentroid;
  float centroid[2];
  float prevCentroid[2];
  float centroidSpeed;
  unsigned long lastCentroidDate;
  unsigned long lastPwmMsgDate;

  // for the chenillard
  int autoCoilCnt;

  ControlMode mode;
  float targetValues[MAX_NUMBER_OF_COILS];

  // interface for a chain of pca9685 pwm controllers
  PWMInterface pwms;
  
  // server
  WebServer *server;

public:
  CoilMapper() :
  nbCoils(0), nbValidDistances(0), incomingCentroid(false),
  autoCoilCnt(0),
  mode(ControlModeGesture) {
    for (int i = 0; i < MAX_NUMBER_OF_COILS; i++) {
      coils[i][0] = coils[i][1] = targetValues[i] = 0;
    }

    centroid[0] = centroid[1] = 0;
    prevCentroid[0] = prevCentroid[1] = 0;
    centroidSpeed = 0;
    lastCentroidDate = lastPwmMsgDate = millis();
  }

  ~CoilMapper() {}

  void init(WebServer *s);
  void setNbCoils(int n);
  void setCoil(int i, float x, float y);
  void setCentroid(float x, float y);
  void setMode(ControlMode m);
  void update();

private:
  void updateGesture();
  void updateAutomatic();
  void computeCentroidSpeed();
  void getClosest(float x, float y);
  float getDistance(float x1, float x2, float y1, float y2);
  void computeGesturePwmTargetValues(float pwmValue);
  void dispatchPwmTargetValues();
  void updateLocalPwmValues();
};

#endif /* _SOFTMIRROR_COILMAPPER_H_ */
