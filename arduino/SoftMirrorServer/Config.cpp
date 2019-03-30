#include <FS.h>
#include "CoilMapper.h"
#include "Config.h"

void
Config::init(CoilMapper *m) {
  if (SPIFFS.begin()) {
    enabled = true;
#ifdef USE_SERIAL
  Serial.println("SPIFFS started");
#endif /* USE_SERIAL */
  }

  mapper = m;
  loadCoils();
}

void
Config::loadCoils() {
  if (!enabled) return;
  bool fExists = SPIFFS.exists("/coilmap.txt");
  File file = SPIFFS.open("/coilmap.txt", "r");

  if (file && fExists) {
    String next = file.readStringUntil('\n');

    if (next == "maxdims") {
      int x = file.readStringUntil('\n').toInt();
      int y = file.readStringUntil('\n').toInt();
      setMaxDims(x, y);
      next = file.readStringUntil('\n');
    }

    if (next == "startmap") {
      nbCoils = 0;
      next = file.readStringUntil('\n');
    
      while (next != "endmap") {
        int x = next.toInt();
        int y = file.readStringUntil('\n').toInt();
        int i = file.readStringUntil('\n').toInt();
        setCoil(i, x, y);
        nbCoils++;
        next = file.readStringUntil('\n');
      }
    }

    mapper->setNbCoils(nbCoils);
  }
}

void
Config::setMaxDims(int x, int y) {
  maxDims[0] = x;
  maxDims[1] = y;
}

void
Config::setCoil(int i, int x, int y) {
  if (i > MAX_NUMBER_OF_COILS) return;
  
  coils[i - 1][0] = x;
  coils[i - 1][1] = y;

  mapper->setCoil(i - 1, (float) x / maxDims[0], (float) y / maxDims[1]);
}

int
Config::getNbCoils() {
  return nbCoils;
}

void
Config::setNbCoils(int n) {
  nbCoils = n;
}

void
Config::writeCoilMapString(char *res) {
  if (!enabled) return;
#ifdef USE_SERIAL
  Serial.println("SPIFFS is enabled");
#endif /* USE_SERIAL */

  String tmp = "coilmap\n";
  bool fExists = SPIFFS.exists("/coilmap.txt");
  File file = SPIFFS.open("/coilmap.txt", "r");

  if (file && fExists) {
#ifdef USE_SERIAL
  Serial.println("coilmap.txt exists");
#endif /* USE_SERIAL */

    strcpy(res, tmp.c_str());
    while (file.available()) {
      tmp = file.readStringUntil('\n') + "\n";
      strcat(res, tmp.c_str());
    }
  }      
}

void
Config::storeCoils() {
  if (!enabled) return;

  if (nbCoils == 0) {
    SPIFFS.format();
  }
  File file = SPIFFS.open("/coilmap.txt", "w+");

  // we don't use file.println() as it seems to add '\r' characters before '\n'
  // and we get them in results of file.readStringUntil('\n')
  // todo: update movuino firmware to correct his
  String line;
  
  if (file) {
    line = String("maxdims\n");
    file.print(line);
    line = String(maxDims[0]) + "\n";
    file.print(line);
    line = String(maxDims[1]) + "\n";
    file.print(line);
    line = String("startmap\n");
    file.print(line);

    for (int i = 0; i < nbCoils; i++) {
      line = String(coils[i][0]) + "\n";
      file.print(line);
      line = String(coils[i][1]) + "\n";
      file.print(line);
      line = String(i + 1) + "\n";
      file.print(line);
    }

    line = String("endmap\n");
    file.print(line);
    file.close();
  }
}

