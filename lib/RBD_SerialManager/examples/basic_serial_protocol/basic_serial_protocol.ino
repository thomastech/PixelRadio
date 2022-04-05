// Arduino RBD Serial Manager Library v1.0.0 Example - Quickly define and consume an event-based serial protocol.
// https://github.com/alextaujenis/RBD_SerialManager
// Copyright (c) 2015 Alex Taujenis - MIT License

#include <RBD_SerialManager.h> // https://github.com/alextaujenis/RBD_SerialManager

RBD::SerialManager serial_manager;

void setup() {
  serial_manager.start();
}

void loop() {
  // you must set the serial monitor to include a newline with each command
  if(serial_manager.onReceive()) {
    // example command: on
    if(serial_manager.isCmd("on")) {
      serial_manager.println("IT'S ON!");
    }

    // example command: pwm,123
    if(serial_manager.isCmd("pwm")) {
      int value = serial_manager.getParam().toInt();
      serial_manager.print("SET PWM ");
      serial_manager.println(value);
    }
  }
}