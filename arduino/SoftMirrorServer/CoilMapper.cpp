#include <Arduino.h>
#include "WebServer.h"
#include "CoilMapper.h"

void
CoilMapper::init(WebServer *s) {
  server = s;
}

void
CoilMapper::setNbCoils(int n) {
  nbCoils = n;
}

// index is decremented and coords are normalized by the Config class
// in Config::setCoil when it calls this method
// Config::setCoil is called by Config::loadCoils and WebServer::storeCoilMap
// this guarantees coherence between the loaded model and the stored map.
void
CoilMapper::setCoil(int i, float x, float y) {
  coils[i][0] = x;
  coils[i][1] = y;
}

void
CoilMapper::setCentroid(float x, float y) {
  centroid[0] = x;
  centroid[1] = y;

  unsigned long now = millis();

  if (incomingCentroid) { // else centroidSpeed already equals zero
    unsigned long deltaTime = now - lastCentroidDate;
    float dist = getDistance(centroid[0], centroid[1], prevCentroid[0], prevCentroid[1]);  
    // centroidSpeed = dist * 1000 / deltaTime; // speed in "unit" per second
    // smoothed centroid speed :
    centroidSpeed = dist * 1000 / deltaTime + centroidSpeed * CENTROID_SPEED_SMOOTH_FACTOR;
  }
  
  prevCentroid[0] = centroid[0];
  prevCentroid[1] = centroid[1];
  lastCentroidDate = now;
  incomingCentroid = true;
}

void
CoilMapper::setMode(ControlMode m) {
  mode = m;
}

void
CoilMapper::update() {
  for (int i = 0; i < MAX_NUMBER_OF_COILS; i++) {
    targetValues[i] = 0;
  }

  if (mode == ControlModeGesture) {
    updateGesture();
  } else {
    updateAutomatic();
  }
}

void
CoilMapper::updateGesture() {
  getClosest(centroid[0], centroid[1]);

  unsigned long now = millis();

  if (now - lastCentroidDate > MAX_CENTROID_MSG_PERIOD && incomingCentroid) {
    centroidSpeed = 0;
    incomingCentroid = false;
  }

  if (!incomingCentroid) {
    lastCentroidDate = now; 
  }

  int pwmSum = 0;
  
  if (centroidSpeed > 1e-3) { // check for float nullity
    //*
    float normPwmSum = CLIP(centroidSpeed, CENTROID_SPEED_LO_THRESH, CENTROID_SPEED_HI_THRESH);
    normPwmSum = SCALE(normPwmSum, CENTROID_SPEED_LO_THRESH, CENTROID_SPEED_HI_THRESH, 0, 1);
    // eventually reshape normPwmSum value here
    // normPwmSum = pow(normPwmSum, 2);
    // normPwmSum = sqrt(normPwmSum);

    normPwmSum *= normPwmSum;
    pwmSum = (int) SCALE(normPwmSum, 0, 1, MIN_PWM_VALUE, MAX_PWM_VALUE);
    //*/
    
    //pwmSum = MAX_PWM_VALUE;
  }
  
  if (now - lastPwmMsgDate > PWM_CONTROL_MSG_PERIOD) {    
    computeGesturePwmTargetValues(pwmSum);
    dispatchPwmTargetValues();
    lastPwmMsgDate = now;
  }

  updateLocalPwmValues();

  // assumption here is that linear mapping from a set of values to another set of values,
  // both having a sum that never exceeds a MAX value, will never result in an intermediary
  // sum of values exceeding this MAX value
  
  // algo :
  // - check centroid velocity (including the fact that if packets stop incoming,
  //   then consider velocity to be null) :
  //   - if below minThresh, fade all coils to zero
  //   - if above minThresh and below maxThresh, dispatch pwm values so that their sum is
  //     velocity mapped from [minThresh, maxThresh] to [MIN_PWM_VALUE, MAX_PWM_VALUE]
  //   - if above maxThresh, dispatch pwm value so that their sum is MAX_PWM_VALUE
}

void
CoilMapper::updateAutomatic() {
  unsigned long now = millis();

  if (now - lastPwmMsgDate > PWM_CONTROL_MSG_PERIOD) {    
    int coilId = autoCoilCnt / 2;
    int coilState = autoCoilCnt % 2;
    
    if (coilState == 0) {
      int prevCoilId = (coilId == 0) ? (nbCoils - 1) : (coilId - 1);
      targetValues[prevCoilId] = 0;
      targetValues[coilId] = MAX_PWM_VALUE;
    } else {
      targetValues[coilId] = MIN_PWM_VALUE;
    }

    autoCoilCnt = (autoCoilCnt + 1) % (2 * nbCoils);
    
    dispatchPwmTargetValues();
    lastPwmMsgDate = now;
  }

  updateLocalPwmValues();  
}

void
CoilMapper::getClosest(float x, float y) {
  nbValidDistances = 0;
  
  for (int i = 0; i < nbCoils; i++) {
    float dist = getDistance(x, y, coils[i][0], coils[i][1]);
    
    if (dist < CENTROID_DIST_THRESH) {
      distances[nbValidDistances] = dist;
      distanceIndexes[nbValidDistances] = i;
      nbValidDistances++;
    }
  }

  // perform in-place sorting :
  
  nbClosest = MAX_NB_CLOSEST_COILS < nbValidDistances ? MAX_NB_CLOSEST_COILS : nbValidDistances;

  for (int i = 0; i < nbClosest; i++) {
    float minDist = distances[i];
    closestIndexes[i] = distanceIndexes[i];

    for (int j = i + 1; j < nbValidDistances; j++) {
      if (distances[j] < minDist) {
        float tmpDist = distances[j];
        distances[j] = distances[i];
        distances[i] = tmpDist;
        minDist = tmpDist;

        int tmpIndex = distanceIndexes[j];
        distanceIndexes[j] = distanceIndexes[i];
        distanceIndexes[i] = tmpIndex;
        closestIndexes[i] = tmpIndex;
      }
    }
  }

#ifdef USE_SERIAL
  /*
  Serial.println("closest indexes : ");
  for (int i = 0; i < nbClosest; i++) {
    Serial.printf("%i\n", closestIndexes[i]);
  }
  //*/

  /*
  Serial.println("distance values : ");
  for (int i = 0; i < nbValidDistances; i++) {
    Serial.printf("%i : %i\n", distanceIndexes[i], (int) (distances[i] * 1000));
  }
  //*/
#endif /* USE_SERIAL */
}

float
CoilMapper::getDistance(float x1, float y1, float x2, float y2) {
  float deltax = x1 - x2;
  float deltay = y1 - y2;
  // return (float) pow(deltax * deltax + deltay * deltay, 0.5f);
  return sqrt(deltax * deltax + deltay * deltay);
}

void
CoilMapper::computeGesturePwmTargetValues(int pwmValue) {
  int i;
  
  if (pwmValue > 0) {
    float closestDistances[nbClosest];
    float distSum = 0;
    
    for (i = 0; i < nbClosest; i++) {
      closestDistances[i] = distances[closestIndexes[i]];
      distSum += closestDistances[i];
    }
  
    for (i = 0; i < nbClosest; i++) {
      closestDistances[i] = closestDistances[i] / distSum;
      targetValues[closestIndexes[i]] = (int) (closestDistances[i] * pwmValue);
    }
  }  
}

void
CoilMapper::dispatchPwmTargetValues() {
  int i;

  for (i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
    coilRamps[i]->setTargetPwmValue(targetValues[i]); // set local target pwm values
  }

  int coilFrame[PWM_OUTPUTS_PER_NODEMCU];
  int boardIndex = 1;

  for (i = PWM_OUTPUTS_PER_NODEMCU; i < nbCoils; i++) {
    int coilIndex = i % PWM_OUTPUTS_PER_NODEMCU;
    coilFrame[coilIndex] = targetValues[i];

    if (i % PWM_OUTPUTS_PER_NODEMCU == PWM_OUTPUTS_PER_NODEMCU - 1 || i == nbCoils - 1) {
      server->sendCoilFrame(i / PWM_OUTPUTS_PER_NODEMCU, &(coilFrame[0])); // set remote target pwm values
      for (int j = 0; j < PWM_OUTPUTS_PER_NODEMCU; j++) {
        coilFrame[j] = 0;
      }
    }
  }
}

void
CoilMapper::updateLocalPwmValues() {
  for (int i = 0; i < PWM_OUTPUTS_PER_NODEMCU; i++) {
    coilRamps[i]->update();
  }
}

