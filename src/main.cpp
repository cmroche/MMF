#ifndef UNIT_TEST

#include <Arduino.h>
#include <timer.h>

#include "triggerevent.h"

// Example: https://create.arduino.cc/projecthub/electropeak/arduino-l293d-motor-driver-shield-tutorial-c1ac9b
// Arduino-timer: https://github.com/contrem/arduino-timer

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(500);
}

#endif