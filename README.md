# Soft Mirror

### e-textile based animatronic system by Claire Eliot

This project contains several software components :

* The `app` directory contains a cordova based application for smartphones and tablets, targeting android and ios platforms. It is aimed at controlling Soft Mirror systems using the OSC protocol. It has two modes : `gesture control` and `automatic`. In `gesture control` mode it will stream the position of the device orientation centroid to drive the movement of the mechanical parts, and in `automatic` mode it will make all those mechanical parts move one after another.

* The `arduino` directory contains an arduino project, `SoftMirror`, for use with ESP8266 based NodeMCU boards connected to a chain of PCA9685 PWM control Adafruit modules. This firmware starts a WiFi network named " softmirror" in access point mode and has two main functions :
    * allow any device (preferably computers) to connect and visit a configuration web page at the address `192.168.1.1`, from which each coil position can be defined on a 2D map (this configuration can be saved to the board's file system and will persist on reboot)
    * allow the device running the control app to connect, receive OSC messages from it, and translate these messages to pwm instructions that will be dispatched to all PCA9685 boards.

There are several global "fine-tuning" parameters located in `arduino/SoftMirror/globals.h` to play with. Have a look at them.

