#ifndef HEIGHTCONTROL_H
#define HEIGHTCONTROL_H

/*
  Class to control the height of the multicopter.
  It uses either the pressure sensor (BMP) for greater heights or the USR for
  start and landing.
*/

/*==================================================================*/
  //Extern librarys
  #include "../params.h"
  #include "Arduino.h"

/*==================================================================*/
  //Classdefinition
  class HEIGHTCONTROL{
  private:
    //further variables
    float U;       //controlled variable (CV)
    float iSum;    //controlled integral variabel
    float old_e;   //used for DController
    float e;       //height difference (W-Y)=(Sollwert-Istwert)

    //private Functions
    float IController(float e, float looptime);
    float DController(float e, float k);

  public:
    //control constants
    float K_P;
    float T_I;
    float T_D;

    float targetHeight;

    void begin(void);
    void update(float RotorSignal[4], float height, float looptime);
    void setConstantsViaSerial(void);
    void setTargetHeight(void);
  };

/*==================================================================*/
  //Private Functions
  float HEIGHTCONTROL::IController(float e, float looptime){
    iSum = iSum + e;
    return (1/T_I)*iSum*(looptime/1000);
  }

  float HEIGHTCONTROL::DController(float e, float looptime){
    float DValue = T_D*(e-old_e)/(looptime/1000);
    old_e = e;
    return DValue;
  }

/*==================================================================*/
  //Public Functions
  void HEIGHTCONTROL::begin(void){
    //initialize I_Controller that makes the copter take off and reacts to more weight
    iSum = 0;

    //initialize D_Controller
    old_e = 0;

    //initialize PID Constants
    K_P = K_P_HEIGHT_START;
    T_I = T_I_HEIGHT_START;
    T_D = T_D_HEIGHT_START;

    //initialize targetHeight (ReferenceHeight could be read from sensor)
    targetHeight = TARGETHEIGHTSTART+REFERENCEHEIGHT;
  }

void HEIGHTCONTROL::update(float RotorSignal[4], float height, float looptime){
  //height difference (W-Y)
  e = targetHeight - height;

  //controlled variabel (CV)
  U = K_P * (e + DController(e, looptime) + IController(e, looptime));

  //add or subtract to RotorSignal
  RotorSignal[0] = RotorSignal[0] + U;
  RotorSignal[1] = RotorSignal[1] + U;
  RotorSignal[2] = RotorSignal[2] + U;
  RotorSignal[3] = RotorSignal[3] + U;
}

void HEIGHTCONTROL::setConstantsViaSerial(void){
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

    void HEIGHTCONTROL::setTargetHeight(void){

      Serial.print("Which height would you like to fly in cm? ");
      Serial.print("It Should be a number with 3 significant characters, for example 0.25 ");
      //wating for Serial to have four digits
      while (Serial.available()<4) {}

      float signalInput = Serial.parseFloat();
      Serial.println(signalInput);
      targetHeight = signalInput+REFERENCEHEIGHT;
    }

#endif
