// Arduino RBD Serial Manager Library v1.0.0 Example - Control the rotation of a servo with serial commands: up down move,90
// https://github.com/alextaujenis/RBD_SerialManager
// Copyright (c) 2015 Alex Taujenis - MIT License

#include <RBD_Servo.h>         // https://github.com/alextaujenis/RBD_Servo
#include <RBD_SerialManager.h> // https://github.com/alextaujenis/RBD_SerialManager

RBD::Servo servo(2, 700, 2300); // digital pin 2, 700ms - 2300ms pulse range
RBD::SerialManager usb;

void setup() {
  usb.start();
}

void loop() {
  // you must set the serial monitor to include a newline with each command
  if(usb.onReceive()) {
    // example serial command: up
    if(usb.isCmd("up")) {
      servo.moveToDegrees(180);
    }

    // example serial command: down
    else if(usb.isCmd("down")) {
      servo.moveToDegrees(0);
    }

    // example serial command: move,90
    else if(usb.isCmd("move")) {
      int _position = usb.getParam().toInt();
      servo.moveToDegrees(_position);
    }
  }
  servo.update();
}