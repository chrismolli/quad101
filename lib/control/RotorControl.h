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
  float RoundSignal[4];
  void begin(void);
  void update(void);
  void start(void);
  void stop(void);
  void setRotorSignalViaSerial(void);
  void sendSerial(void);
};


void RotorControl::begin(void){
  //Establish Connection to ESCs
  esc1.attach(PWMOUT1);
  esc2.attach(PWMOUT2);
  //Initialize Rotors and be careful with delays!!
  esc1.writeMicroseconds(0);
  esc2.writeMicroseconds(0);
  delay(500);

  //Initialize RotorSignal
  RotorSignal[0] = 0;
  RotorSignal[1] = 0;
  RotorSignal[2] = 0;
  RotorSignal[3] = 0;

  RoundSignal[0] = 0;
  RoundSignal[1] = 0;
  RoundSignal[2] = 0;
  RoundSignal[3] = 0;
}

void RotorControl::start(void){
  //Startvorgang Teststand
  int s = MIN_ROTOR_SIGNAL;
  while (s < TAKE_OFF_SIGNAL){
    esc1.writeMicroseconds(s);
    esc2.writeMicroseconds(s);
    s = s + 20;
    delay(100);
  }

  esc1.writeMicroseconds(TAKE_OFF_SIGNAL);
  esc2.writeMicroseconds(TAKE_OFF_SIGNAL);

  //Write start value to the RotorSignals
  RotorSignal[0] = TAKE_OFF_SIGNAL;
  RotorSignal[1] = TAKE_OFF_SIGNAL;

}

void RotorControl::stop(void){
  //Startvorgang Teststand
  int s = TAKE_OFF_SIGNAL;
  while (s > MIN_ROTOR_SIGNAL){
    esc1.writeMicroseconds(s);
    esc2.writeMicroseconds(s);
    s = s - 20;
    delay(40);
  }

  esc1.writeMicroseconds(0);
  esc2.writeMicroseconds(0);

  //Reset RotorSignal
  RotorSignal[0] = 0;
  RotorSignal[1] = 0;

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
  RoundSignal[0] = roundf(RotorSignal[0]);
  RoundSignal[1] = roundf(RotorSignal[1]);
  esc1.writeMicroseconds((int)RoundSignal[0]);
  esc2.writeMicroseconds((int)RoundSignal[1]);
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

void RotorControl::sendSerial(void){
    //Prints the time and RotorSignals to serial
    Serial.println("Time: ");
    Serial.print(millis());
    Serial.print(" R0: ");
    Serial.print(RotorSignal[0]);
    Serial.print(" R1: ");
    Serial.print(RotorSignal[1]);
    /*Serial.print(" R2: ");
    Serial.print(RotorSignal[2]);
    Serial.print(" R3: ");
    Serial.print(RotorSignal[3]); */
  }

#endif
