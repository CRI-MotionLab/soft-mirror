# Soft Mirror

### e-textile based animatronic system by Claire Eliot

This project contains several software components :

* SoftMirrorApp is a cordova based application for smartphones and tablets, targeting android and ios platforms. It is aimed at controlling Soft Mirror systems using the OSC protocol.
* SoftMirrorFirmware is currently a firmware for teensy boards, reading input from the SoftMirrorApp (currently forwarded from a local software receiving SoftMirrorApp's OSC messages and translating them to serial packets for the teensy). The next version should be based on a wifi enabled board such as the Adafruit feather or node-mcu, to allow the mobile app to connect directly to the board via wifi.
