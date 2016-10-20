#ifndef HEIGHTCONTROL_H
#define HEIGHTCONTROL_H

/*==================================================================*/
  //Extern librarys
  #include "../params.h"
  #include "Arduino.h"

/*==================================================================*/
  //Classdefinition
  class HEIGHTCONTROL{
  private:
    //further variables
    float U_height; //controlled variable (CV)
    float iSum; //controlled integral variabel
    float old_e_height; //used for DController
    float e_height; //height difference (W_Y)
    //W = Sollwert
    //Y = Istwert

    //private Functions
    float IController(float e, float looptime);
    float DController(float e, float k);

  public:
    //control constants
    float K_P_HEIGHT;
    float T_I_HEIGHT;
    float T_D_HEIGHT;

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
    return (1/T_I_HEIGHT)*iSum*(looptime/1000);
  }

  float HEIGHTCONTROL::DController(float e, float looptime){
    float DValue = T_D_HEIGHT*(e-old_e_height)/(looptime/1000);
    old_e_height = e;
    return DValue;
  }

  //Public Functions
  void HEIGHTCONTROL::begin(void){
    //initialize I_Controller that makes the copter take off and reacts to more weight
    iSum = 0;

    //initialize D_Controller
    old_e_height = 0;

    //initialize PID Constants
    K_P_HEIGHT = K_P_HEIGHT_START;
    T_I_HEIGHT = T_I_HEIGHT_START;
    T_D_HEIGHT = T_D_HEIGHT_START;

    //initialize targetHeight
    targetHeight = TARGETHEIGHTSTART+REFERENCEHEIGHT;
  }

void HEIGHTCONTROL::update(float RotorSignal[4], float height, float looptime){

  e_height = targetHeight - height; //get height difference (W-Y)
  U_height = K_P_HEIGHT * (e_height + DController(e_height, looptime) + IController(e_height, looptime));

  //add or subtract to RotorSignal
  RotorSignal[0] = RotorSignal[0] + U_height;
  RotorSignal[1] = RotorSignal[1] + U_height;
  RotorSignal[2] = RotorSignal[2] + U_height;
  RotorSignal[3] = RotorSignal[3] + U_height;
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
        K_P_HEIGHT = kString.toFloat();
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
        T_I_HEIGHT = kString.toFloat();
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
        T_D_HEIGHT = kString.toFloat();
        Serial.println(kString);
        break;

      default:
        Serial.println("This constant doesn't exist. Start over again");
        //while(Serial.available()) Serial.read();
        break;
    }
  }

    void HEIGHTCONTROL::setTargetHeight(void){

      Serial.print("Which height would you like to fly? ");
      Serial.print("It Should be a number with 3 significant characters, for example 0.25 ");
      //wating for Serial to have four digits
      while (Serial.available()<4) {}

      float signalInput = Serial.parseFloat();
      Serial.println(signalInput);
      targetHeight = signalInput+REFERENCEHEIGHT;
    }

#endif
