#ifndef PIDCONTROL_H
#define PIDCONTROL_H

/*==================================================================*/
  //Extern librarys
  #include "../params.h"
  #include "Arduino.h"

/*==================================================================*/
  //Classdefinition
  class PIDControl{
    private:
      float K_P;
      float T_I;
      float T_D;

      float K_P_Jaw;
      float T_I_Jaw;
      float T_D_Jaw;

      float U[3]; //controlled variable (CV)
      float iSum[3]; //controlled integral variabel
      float e[3]; //control difference (W_Y)
      //W = Sollwert
      //Y = Istwert

    public:
      float PController(float e, float k);
      float IController(float e, float k, int i, float looptime);
      float DController(float dE, float k);
      void begin(void);
      void update(float RotorSignal[4], float Y[3], float dE[3], float W[3], float looptime);
      void setConstantsViaSerial(void);
  };

/*==================================================================*/
  //Functions
float PIDControl::PController(float e, float k){
  return k*e;
}

float PIDControl::IController(float e, float k, int i, float looptime){
  if ((abs(e) < MAX_E_FOR_I) && (abs(e) > MIN_E_FOR_I)) {
     iSum[i] = iSum[i] + e;
  }
  return (1/k)*iSum[i]*looptime/1000;
}

float PIDControl::DController(float dE, float k){
  return -k * dE; //sign has to match PController
}

void PIDControl::begin(void){
  //initialize I_Controller
  iSum[0] = 0;
  iSum[1] = 0;
  iSum[2] = 0;

  //initialize PID Constants
  K_P = K_P_START;
  T_I = T_I_START;
  T_D = T_D_START;

  K_P_Jaw = 0.1;
  T_I_Jaw = 0.05;
  T_D_Jaw = 0.1;
}

void PIDControl::update(float RotorSignal[4], float Y[3], float dE[3], float W[3], float looptime){
  //calculate control difference
  e[0] = W[0]-Y[0];
  e[1] = W[1]-Y[1];
  e[2] = W[2]-Y[2];

  //calculating cotrolled variable (CV)
  //x-Axis
  U[0] = K_P*(PController(e[0], 1) + IController(e[0], T_I, 0, looptime) + DController(dE[0], T_D));

  //y-Axis
  //U[1] = PController(e[1], K_P) + IController(e[1], T_I, 1, looptime) + DController(dE[1], T_D);

  //z-Axis
  //U[2] = PController(e[2], K_P_Jaw) + IController(e[2], T_I_Jaw, 2, looptime) + DController(dE[2], T_D_Jaw);

  //Multiplication with System_Matrice
  //U[0] = 1 * U[0];
  //U[1] = 1 * U[1];
  //U[2] = 49.271 * U[2];

  //x_axis
  RotorSignal[0] = RotorSignal[0] + U[0]; //0 muss also in positiver Winkelrichtung liegen
  RotorSignal[1] = RotorSignal[1] - U[0];
  //y_axis
  //RotorSignal[2] = RotorSignal[2] + U[1];
  //RotorSignal[3] = RotorSignal[3] - U[1];
  //z-axis; counterclockwise = positive; Moment wirkt entgegen der Drehrichtung der Rotoren
  //RotorSignal[0] = RotorSignal[0] + U[2]; //1 und 2 müssen sich counterclockwise drehen --> Moment clockwise
  //RotorSignal[1] = RotorSignal[1] + U[2];
  //RotorSignal[2] = RotorSignal[2] - U[2];
  //RotorSignal[3] = RotorSignal[3] - U[2];
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
        Serial.println("Set new T_I value: ");
        while(inChar!='\n'){
          if (Serial.available()){
            inChar = (char)Serial.read();
            if(inChar == 'c'){ // 'c' resets kString so you can input value again
              kString = "";
              //while(Serial.available()) Serial.read();
              Serial.println("T_I value got reset!");
              Serial.println("Set new T_I value: ");
            }
            else if (inChar != '\n'){
              kString += inChar;
              Serial.println(kString);
            }
          }
        }
        Serial.print("Your new T_I value is: ");
        T_I = kString.toFloat();
        Serial.println(kString);
        break;

      case 100: // compares input to 'd'
        Serial.println("Set new T_D value: ");
        while(inChar!='\n'){
          if (Serial.available()){
            inChar = (char)Serial.read();
            if(inChar == 'c'){ // 'c' resets kString so you can input value again
              kString = "";
              //while(Serial.available()) Serial.read();
              Serial.println("T_D value got reset!");
              Serial.println("Set new T_D value: ");
            }
            else if (inChar != '\n'){
              kString += inChar;
              Serial.println(kString);
            }
          }
        }
        Serial.print("Your new T_D value is: ");
        T_D = kString.toFloat();
        Serial.println(kString);
        break;

      default:
        Serial.println("This constant doesn't exist. Start over again");
        //while(Serial.available()) Serial.read();
        break;
    }
  }

#endif
