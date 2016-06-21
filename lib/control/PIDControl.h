#ifndef PIDCONTROL_H
#define PIDCONTROL_H

#include "../params.h"
#include "Arduino.h"


class PIDControl{
private:
  float K_P;
  float K_I;
  float K_D;

  float K_P_Jaw;
  float K_I_Jaw;
  float K_D_Jaw;

  float U[3]; //controlled variable (CV)
  float iSum[3]; //controlled integral variabel
  float e[3]; //control difference (W_Y)
  //W = Sollwert
  //Y = Istwert

public:
  float PController(float e, float k);
  float IController(float e, float k, int i, float looptime);
  float DController(float dE, float k);
  void set(void);
  void update(float RotorSignal[4], float Y[3], float dE[3], float W[3], float looptime);
  void sendSerial(float RotorSignal[4]);
  void setConstantsViaSerial(void);
};

// functions for each individual regulation
float PIDControl::PController(float e, float k){
  return k*e;
}

float PIDControl::IController(float e, float k, int i, float looptime){
  if ((abs(e) < MAX_E_FOR_I) && (abs(e) > MIN_E_FOR_I)) {
     iSum[i] = iSum[i] + e;
  }
  return k*iSum[i]*looptime/1000;
}

float PIDControl::DController(float dE, float k){
  return -k * dE; //sign has to match PController
}

void PIDControl::set(void){
  //initialize I_Controller
  iSum[0] = 0;
  iSum[1] = 0;
  iSum[2] = 0;
  //initialize Constants
  K_P = 0.0000215; //0.000027
  K_I = 0.00000001; //0.0000001
  K_D = 0.0225; //0.022 (Filtered derivative and 100Hz)

  K_P_Jaw = 0.1;
  K_I_Jaw = 0.05;
  K_D_Jaw = 0.1;
}

void PIDControl::update(float RotorSignal[4], float Y[3], float dE[3], float W[3], float looptime){
  //calculate control difference
  e[0] = W[0]-Y[0];
  e[1] = W[1]-Y[1];
  e[2] = W[2]-Y[2];

  //calculating cotrolled variable (CV)
  //x-Axis
  U[0] = PController(e[0], K_P) + IController(e[0], K_I, 0, looptime) + DController(dE[0], K_D);

  //y-Axis
  U[1] = PController(e[1], K_P) + IController(e[1], K_I, 1, looptime) + DController(dE[1], K_D);

  //z-Axis
  U[2] = PController(e[2], K_P_Jaw) + IController(e[2], K_I_Jaw, 2, looptime) + DController(dE[2], K_D_Jaw);

  //Multiplication with System_Matrice
  U[0] = 1 * U[0];
  U[1] = 1 * U[1];
  //U[2] = 49.271 * U[2];

  //x_axis
  RotorSignal[0] = RotorSignal[0] + U[0]; //0 muss also in positiver Winkelrichtung liegen
  RotorSignal[1] = RotorSignal[1] - U[0];
  //y_axis
  RotorSignal[2] = RotorSignal[2] + U[1];
  RotorSignal[3] = RotorSignal[3] - U[1];
  //z-axis; counterclockwise = positive; Moment wirkt entgegen der Drehrichtung der Rotoren
  //RotorSignal[0] = RotorSignal[0] + U[2]; //1 und 2 müssen sich counterclockwise drehen --> Moment clockwise
  //RotorSignal[1] = RotorSignal[1] + U[2];
  //RotorSignal[2] = RotorSignal[2] - U[2];
  //RotorSignal[3] = RotorSignal[3] - U[2];
}

void PIDControl::sendSerial(float RotorSignal[4]){
    //Prints the time and RotorSignals to serial
    Serial.println("Time: ");
    Serial.print(millis());
    Serial.print(" R0: ");
    Serial.print(RotorSignal[0]);
    Serial.print(" R1: ");
    Serial.print(RotorSignal[1]);
    Serial.print(" R2: ");
    Serial.print(RotorSignal[2]);
    Serial.print(" R3: ");
    Serial.print(RotorSignal[3]);
  }

void PIDControl::setConstantsViaSerial(void){
  Serial.println("Tell me which k_Value you want to update ");
  while (!Serial.available()) {}
  String kString = "";
  char inChar = (char)Serial.read();
  switch (inChar) {
    case 112: // compares input to 'p'
      Serial.println("Set new K_P value: ");
        while(inChar != '\n'){
            if (Serial.available()){
              inChar = (char)Serial.read();
              if(inChar == 'c'){ // 'c' resets kString so you can input value again
                kString = "";
                //while(Serial.available()) Serial.read();
                Serial.println("K_P value got reset!");
                Serial.println("Set new K_P value: ");
              }
              else if (inChar != '\n'){
                kString += inChar;
                Serial.println(kString);
              }
            }
          }
        Serial.print("Your new K_P value is: ");
        K_P = kString.toFloat();
        Serial.println(kString);
        break;
      case 105: // compares input to 'i'
        Serial.println("Set new K_I value: ");
        while(inChar!='\n'){
          if (Serial.available()){
            inChar = (char)Serial.read();
            if(inChar == 'c'){ // 'c' resets kString so you can input value again
              kString = "";
              //while(Serial.available()) Serial.read();
              Serial.println("K_I value got reset!");
              Serial.println("Set new K_I value: ");
            }
            else if (inChar != '\n'){
              kString += inChar;
              Serial.println(kString);
            }
          }
        }
        Serial.print("Your new K_I value is: ");
        K_I = kString.toFloat();
        Serial.println(kString);
        break;
      case 100: // compares input to 'd'
      Serial.println("Set new K_D value: ");
        while(inChar!='\n'){
          if (Serial.available()){
            inChar = (char)Serial.read();
            if(inChar == 'c'){ // 'c' resets kString so you can input value again
              kString = "";
              //while(Serial.available()) Serial.read();
              Serial.println("K_D value got reset!");
              Serial.println("Set new K_D value: ");
            }
            else if (inChar != '\n'){
              kString += inChar;
              Serial.println(kString);
            }
          }
        }
        Serial.print("Your new K_D value is: ");
        K_D = kString.toFloat();
        Serial.println(kString);
        break;
      default:
        Serial.println("This constant doesn't exist. Start over again");
        //while(Serial.available()) Serial.read();
        break;
    }
  }

#endif
