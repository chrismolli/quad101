#ifndef ROTORCONTROL_H
#define ROTORCONTROL_H

#include "Arduino.h"
#include "Servo.h"
#include "../params.h"


class RotorControl{
private:
  Servo esc1, esc2, esc3, esc4;
public:
  float RotorSignal[4];
  void begin(void);
  void update(void);
  void start(void); //mit Serial abfrage??
  void setRotorSignalViaSerial(void);
};


void RotorControl::begin(void){

  esc1.attach(PWMOUT1); //Establish Connection to ESCs
  esc2.attach(PWMOUT2);
  esc1.write(0); //Initialize Rotors and be careful with delays!!
  delay(500);
  esc2.write(0);
  delay(500);
  RotorSignal[0] = 0;
  RotorSignal[1] = 0;
  RotorSignal[2] = 0;
  RotorSignal[3] = 0;
}

void RotorControl::start(void){
  //Startvorgang Teststand
  int s = MIN_ROTOR_SIGNAL;
  while (s < TAKE_OFF_SIGNAL){
    esc1.write(s);
    esc2.write(s);
    s = s + 1;
    delay(100);
  }

  esc1.write(TAKE_OFF_SIGNAL);
  esc2.write(TAKE_OFF_SIGNAL);

  //Write start value to the RotorSignals
  RotorSignal[0] = TAKE_OFF_SIGNAL;
  RotorSignal[1] = TAKE_OFF_SIGNAL;

}

void RotorControl::update(void){

//rotorSignal mustn't exceed limits
  //Rotor 0
  if (RotorSignal[0] > MAX_ROTOR_SIGNAL){ //Maximum Value
    RotorSignal[0] = MAX_ROTOR_SIGNAL;
    //Serial.println("R0 > MAX\n");
  }
  if (RotorSignal[0] < MIN_ROTOR_SIGNAL){ //Minimum Value
    RotorSignal[0] = MIN_ROTOR_SIGNAL;
    //Serial.println("R0 < MIN\n");
  }
  //Rotor 1
  if (RotorSignal[1] > MAX_ROTOR_SIGNAL){
    RotorSignal[1] = MAX_ROTOR_SIGNAL;
    //Serial.println("R1 > MAX\n");
  }
  if (RotorSignal[1] < MIN_ROTOR_SIGNAL){
    RotorSignal[1] = MIN_ROTOR_SIGNAL;
    //Serial.println("R1 < MIN\n");
  }

//write RotorSignal too ESCs
  esc1.write((int)RotorSignal[0]);
  esc2.write((int)RotorSignal[1]);
}

void RotorControl::setRotorSignalViaSerial(void){
  Serial.print("The maximum Signal you can send to the Rotors is: ");
  Serial.println(MAX_ROTOR_SIGNAL);
  Serial.println("Always use 3 digits --> Example: 090");
  Serial.println("Tell me which Signal you want to send to both Rotors. ");
  while (Serial.available()<3) {} //wating for Serial to have three digits
  int signalInput = Serial.parseInt();
  if(signalInput <= MAX_ROTOR_SIGNAL){  //if you want the regulation to be steady
    RotorSignal[0] = signalInput;      //you would need to safe the original RotorSignal and compare them
    RotorSignal[1] = signalInput;      //then you could add the difference!
  }
}

#endif
