# Soft Mirror

### e-textile based animatronic system by Claire Eliot

This project contains several software components :

* `app` is a cordova based application for smartphones and tablets, targeting android and ios platforms. It is aimed at controlling Soft Mirror systems using the OSC protocol.
* `arduino` contains two arduino project for use with esp8266 based node-mcu boards :
    * `SoftMirrorServer` which acts as a WiFi access point for all the other devices used in the project, serves a configuration web page (and persists the configuration to memory when saved), receives OSC messages from the `app`, translates these messages into pwm instructions, dispatches some of these instructions to its own pins, and forwards the others to the other connected client node-mcu boards.
    * `SoftMirrorClient` is the firmware for the other node-mcu boards which will automatically connect to the server on boot. A specific id (from 1 to N) must be changed in the code (the `BOARD_ID` #define in `globalConfig.h`) before flashing each client board.
