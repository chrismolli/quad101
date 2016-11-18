#ifndef RADIOCONTROL_H
#define RADIOCONTROL_H

/*==================================================================*/
  //Extern librarys
  #include "Arduino.h"
  #include "../params.h"
  #include "PositionControl.h"

/*==================================================================*/
  //Classdefinition
  class RADIOCONTROL{
  private:
    int ch1, ch2, ch3, ch4;
    int throttle_old;
    int throttle_difference;

    float mapf(int x, float in_min, float in_max, float out_min, float out_max);

    POSITIONCONTROL* positionController; //needed to change targetPosition

  public:
    void begin(POSITIONCONTROL* pControllerPointer);
    void update(float RotorSignal[4]);
  };

/*==================================================================*/
  //General Functions (begin, start, stop)
void RADIOCONTROL::begin(POSITIONCONTROL* pControllerPointer){
    positionController = pControllerPointer;
    throttle_old = BEFORE_TAKE_OFF_SIGNAL;
    throttle_difference = 0;

    pinMode(CHANNEL1PIN, INPUT);
    pinMode(CHANNEL2PIN, INPUT);
    pinMode(CHANNEL3PIN, INPUT);
    pinMode(CHANNEL4PIN, INPUT);
}

void RADIOCONTROL::update(float RotorSignal[4]){
  /*
  Channel1 for Throttle
  Channel2 for Pitch
  Channel3 for Roll
  Channel4 for JAW
  Channel5 for TurnOff

  Map channel1 Input (1000-2000)
  Map channel2/channel3 Input (1000-2000) to degrees (-45°-45°)
  Map channel4 Input (1000-2000) to degrees (0°-360°)
  */
    ch1 = pulseIn(CHANNEL1PIN, HIGH, 5);
    ch2 = pulseIn(CHANNEL2PIN, HIGH, 5);
    ch3 = pulseIn(CHANNEL3PIN, HIGH, 5);
    ch4 = pulseIn(CHANNEL4PIN, HIGH, 5);

    throttle_difference = map(ch1, 1000, 2000, 1000, 2000)-throttle_old;
    positionController->targetPosition[1] = mapf(ch2, 1000, 2000, -45, 45);
    positionController->targetPosition[0] = mapf(ch3, 1000, 2000, -45, 45);
    positionController->targetPosition[2] = mapf(ch4, 1000, 2000, 0, 360);

    RotorSignal[0] += throttle_difference;
    RotorSignal[1] += throttle_difference;
    RotorSignal[2] += throttle_difference;
    RotorSignal[3] += throttle_difference;
  }

/*==================================================================*/
  //Private functions
float RADIOCONTROL::mapf(int x, float in_min, float in_max, float out_min, float out_max)
{
  return ((float)x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
