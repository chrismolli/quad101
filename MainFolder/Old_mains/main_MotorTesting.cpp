/*
Script for testing motors
*/

#include "Arduino.h"
#include <Timer.h>
#include "../lib/sensorfuse/sensorfuse.h"
#include "../lib/control/PIDControl.h"
#include "../lib/control/RotorControl.h"
#include "../lib/control/HeightControl.h"
#include "../lib/params.h"

RotorControl rotors;
int input;

void setup(){
  //Start Serial and wait for connection
  Serial.begin(38400);
  while(!Serial);

  rotors.begin(); //set Rotors and ESCs to PINs and initialize
}

void loop(){
  Serial.print("Input RotorSignal (1012 - 2012): ");
  while (Serial.available() < 4) {}
  input = Serial.parseInt();
  Serial.println(input, DEC);
  rotors.setESC1(input);
}
