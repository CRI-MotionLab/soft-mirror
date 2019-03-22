#ifndef _SOFTMIRROR_SERVER_CONFIG_H_
#define _SOFTMIRROR_SERVER_CONFIG_H_

#include <Arduino.h>
#include "globalConfig.h"

class CoilMapper;

class Config {
private:
  bool enabled;
  int nbCoils;
  int maxDims[2];
  int coils[MAX_NUMBER_OF_COILS][2];

  CoilMapper *mapper;

public:
  Config() : enabled(false), nbCoils(0) {
    maxDims[0] = maxDims[1] = 0;
    for (int i = 0; i < MAX_NUMBER_OF_COILS; i++) {
      coils[i][0] = coils[i][1] = 0;
    }
  }
  ~Config() {}

  void init(CoilMapper *m);
  void loadCoils();
  void setMaxDims(int x, int y);
  void setCoil(int i, int x, int y);
  int getNbCoils();
  void setNbCoils(int n);
  void writeCoilMapString(char *res);
  void storeCoils();
};

#endif /* _SOFTMIRROR_SERVER_CONFIG_H_ */
