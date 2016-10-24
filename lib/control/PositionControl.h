#ifndef POSITIONCONTROL_H
#define POSITIONCONTROL_H

/*==================================================================*/
  //Extern librarys
  #include "../params.h"
  #include "Arduino.h"

/*==================================================================*/
  //Classdefinition
  class POSITIONCONTROL{
    private:
      //further variables
      float U[3]; //controlled variable (CV)
      float iSum[3]; //controlled integral variabel
      float old_dE[3]; //used for DDController
      float e[3]; //control difference (W_Y)
      //W = Sollwert
      //Y = Istwert

      //private Functions
      float PController(float e, float k);
      float IController(float e, float k, int i, float looptime);
      float DController(float dE, float k);
      float DDController(float dE, float k, int i, float looptime);

    public:
      //Pitch and Roll
      float K_P;
      float T_I;
      float T_D;
      float T_DD;

      //Jaw
      float K_P_JAW;
      float T_I_JAW;
      float T_D_JAW;
      float T_DD_JAW;

      float targetPosition[3];

      void begin(void);
      void update(float RotorSignal[4], float Y[3], float dE[3], float looptime);
      void setConstantsViaSerial(void);
  };


/*==================================================================*/
  //Private Functions
float POSITIONCONTROL::PController(float e, float k){
  return k*e;
}

float POSITIONCONTROL::IController(float e, float k, int i, float looptime){
  if ((abs(e) < MAX_E_FOR_I) && (abs(e) > MIN_E_FOR_I)) {
     iSum[i] = iSum[i] + e;
  }
  return (1/k)*iSum[i]*(looptime/1000);
}

float POSITIONCONTROL::DController(float dE, float k){
  return -k * dE; //sign has to match PController
}

float POSITIONCONTROL::DDController(float dE, float k, int i, float looptime){
  float DDValue = -k*(dE-old_dE[i])/(looptime/1000);
  old_dE[i] = dE;
  return DDValue;
}

/*==================================================================*/
  //Public Functions

void POSITIONCONTROL::begin(void){
  //initialize I_Controller
  iSum[0] = 0;
  iSum[1] = 0;
  iSum[2] = 0;

  //initialize DD_Controller
  old_dE[0] = 0;
  old_dE[1] = 0;
  old_dE[2] = 0;

  //initialize PID Constants
  K_P = K_P_START;
  T_I = T_I_START;
  T_D = T_D_START;
  T_DD = T_DD_START;

  K_P_JAW = K_P_JAW_START;
  T_I_JAW = T_I_JAW_START;
  T_D_JAW = T_D_JAW_START;
  T_DD_JAW = T_DD_JAW_START;
}

void POSITIONCONTROL::update(float RotorSignal[4], float Y[3], float dE[3], float looptime){
  //calculate control difference
  e[0] = targetPosition[0]-Y[0];
  e[1] = targetPosition[1]-Y[1];
  e[2] = targetPosition[2]-Y[2];

  //calculating cotrolled variable (CV)
  //x-Axis
  U[0] = K_P*(PController(e[0], 1) + IController(e[0], T_I, 0, looptime) + DController(dE[0], T_D) + DDController(dE[0], T_DD, 0, looptime));

  //y-Axis
  U[1] = K_P*(PController(e[1], 1) + IController(e[1], T_I, 1, looptime) + DController(dE[1], T_D) + DDController(dE[1], T_DD, 1, looptime));

  //z-Axis
  U[2] = K_P_JAW*(PController(e[2], 1) + IController(e[2], T_I_JAW, 2, looptime) + DController(dE[2], T_D_JAW) + DDController(dE[2], T_DD_JAW, 2, looptime));

  //Multiplication with System_Matrice
  //U[0] = 1 * U[0];
  //U[1] = 1 * U[1];
  //U[2] = 49.271 * U[2];

  //x_axis
  RotorSignal[0] = RotorSignal[0] + U[0]; //0 muss also in positiver Winkelrichtung liegen
  RotorSignal[1] = RotorSignal[1] - U[0];
  //y_axis
  RotorSignal[2] = RotorSignal[2] + U[1];
  RotorSignal[3] = RotorSignal[3] - U[1];
  //z-axis; counterclockwise = positive; Moment wirkt entgegen der Drehrichtung der Rotoren
  RotorSignal[0] = RotorSignal[0] + U[2]; //0 und 1 müssen sich counterclockwise drehen --> Moment clockwise
  RotorSignal[1] = RotorSignal[1] + U[2];
  RotorSignal[2] = RotorSignal[2] - U[2];
  RotorSignal[3] = RotorSignal[3] - U[2];
}


void POSITIONCONTROL::setConstantsViaSerial(void){
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

        case 97: // compares input to 'a'
          Serial.println("Set new T_DD value: ");
          while(inChar!='\n'){
            if (Serial.available()){
              inChar = (char)Serial.read();
              if(inChar == 'c'){ // 'c' resets kString so you can input value again
                kString = "";
                //while(Serial.available()) Serial.read();
                Serial.println("T_DD value got reset!");
                Serial.println("Set new T_DD value: ");
              }
              else if (inChar != '\n'){
                kString += inChar;
                Serial.println(kString);
              }
            }
          }
          Serial.print("Your new T_DD value is: ");
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
