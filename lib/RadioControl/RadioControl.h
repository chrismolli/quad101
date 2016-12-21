#ifndef RADIOCONTROL_H
#define RADIOCONTROL_H

/*
  Class to connect a Radio Transmitter and Receiver to the Flight Controller.
  It takes the CPPM signal from the receiver using one pin and updates the
  the channels
  ch1 Jaw velocity
  ch2 Pitch
  ch3 Throttle
  ch4 Roll
  ch5 arm/disarm
*/

/*==================================================================*/
  //Extern librarys
  #include "Arduino.h"
  #include "../params.h"
  #include "PositionControl.h"

/*==================================================================*/
  //Fill modes
  typedef enum{
    RC_START    = 0,
    RC_CH1      = 1,
    RC_CH2      = 2,
    RC_CH3      = 3,
    RC_CH4      = 4,
    RC_CH5      = 5,
    RC_CH6      = 6
  }rc_fillmodes_t;

/*==================================================================*/
  //Classdefinition
  class RADIOCONTROL{
  private:
    //Signal collection
    static uint32_t pulseStart;
    static uint32_t pulseEnd;
    static uint8_t fillMode;
    static void fillChannels();
    static void collectCPPMcallback();

    //SignalMapping
    POSITIONCONTROL* positionController;
    float throttle_old;
    float throttle_dif;
    float mapf(int x, float in_min, float in_max, float out_min, float out_max);

  public:
    void begin(POSITIONCONTROL* pControllerPointer);
    void update(float RotorSignal[4], float rot[3]);
    void debug();
    float throttle;

    //Channels are filled with signal block length in microseconds
    static uint32_t ch1, ch2, ch3, ch4, ch5, ch6;
  };

  //Initialize static variables
  uint8_t RADIOCONTROL::fillMode=0;
  uint32_t RADIOCONTROL::pulseStart=0;
  uint32_t RADIOCONTROL::ch1=0;
  uint32_t RADIOCONTROL::ch2=0;
  uint32_t RADIOCONTROL::ch3=0;
  uint32_t RADIOCONTROL::ch4=0;
  uint32_t RADIOCONTROL::ch5=0;
  uint32_t RADIOCONTROL::ch6=0;
  uint32_t RADIOCONTROL::pulseEnd=0;

/*==================================================================*/
  //Public functions
void RADIOCONTROL::begin(POSITIONCONTROL* pControllerPointer){
  positionController = pControllerPointer;
  pinMode(RC_CPPM_PIN, INPUT);
  RADIOCONTROL::fillMode=RC_START;
  attachInterrupt(RC_CPPM_PIN,&RADIOCONTROL::collectCPPMcallback,CHANGE);
  throttle=MIN_ROTOR_SIGNAL;
}

void RADIOCONTROL::update(float RotorSignal[4], float rot[3]){
  throttle_old = throttle;
  throttle = mapf(ch3, RC_MIN, RC_MAX, MIN_ROTOR_SIGNAL, MAX_ROTOR_SIGNAL);
  positionController->targetPosition[1] = mapf(ch2, RC_MIN, RC_MAX, -MAXIMUM_ANGLE, MAXIMUM_ANGLE);
  positionController->targetPosition[0] = mapf(ch4, RC_MIN, RC_MAX, -MAXIMUM_ANGLE, MAXIMUM_ANGLE);
  positionController->targetPosition[2] = mapf(ch1, RC_MIN, RC_MAX, -MAXIMUM_JAW_SPEED, MAXIMUM_JAW_SPEED);

  //Compensate loss in throttle due to Roll and Pitch
  if(rot[0] != 90 && rot[1] != 90) throttle = throttle*1/(cos(rot[0]*PI/180)*cos(rot[1]*PI/180));

  throttle_dif = throttle-throttle_old;

  RotorSignal[0] += throttle_dif;
  RotorSignal[1] += throttle_dif;
  RotorSignal[2] += throttle_dif;
  RotorSignal[3] += throttle_dif;
}

void RADIOCONTROL::debug(){
  Serial.print(" CH1: ");
  Serial.print(ch1);
  Serial.print(" CH2: ");
  Serial.print(ch2);
  Serial.print(" CH3: ");
  Serial.print(ch3);
  Serial.print(" CH4: ");
  Serial.print(ch4);
  Serial.print(" CH5: ");
  Serial.print(ch5);
  Serial.print(" CH6: ");
  Serial.println(ch6);
}

/*==================================================================*/
  //Private functions
void RADIOCONTROL::collectCPPMcallback(){
  switch (digitalRead(RC_CPPM_PIN)) {
    case HIGH:
      pulseStart=micros();
      break;
    case LOW:
      pulseEnd=micros();
      fillChannels();
  }
}

void RADIOCONTROL::fillChannels(){
  uint32_t rcPulseTime= pulseEnd-pulseStart;
  if(rcPulseTime>=RC_START_BLOCKLENGTH) fillMode=RC_START;
  switch (fillMode) {
    case RC_CH1:
      ch1=rcPulseTime;
      fillMode=RC_CH2;
      break;
    case RC_CH2:
      ch2=rcPulseTime;
      fillMode=RC_CH3;
      break;
    case RC_CH3:
      ch3=rcPulseTime;
      fillMode=RC_CH4;
      break;
    case RC_CH4:
      ch4=rcPulseTime;
      fillMode=RC_CH5;
      break;
    case RC_CH5:
      ch5=rcPulseTime;
      fillMode=RC_CH6;
      break;
    case RC_CH6:
      ch6=rcPulseTime;
      fillMode=RC_START;
      break;
    default:
      fillMode=RC_CH1;
  }
}

float RADIOCONTROL::mapf(int x, float in_min, float in_max, float out_min, float out_max)
{
  return ((float)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
