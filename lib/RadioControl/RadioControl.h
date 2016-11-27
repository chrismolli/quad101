#ifndef RADIOCONTROL_H
#define RADIOCONTROL_H

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
    static uint8_t rcDataRdy;
    //Channels are filled with signal block length in microseconds
    static uint32_t ch1, ch2, ch3, ch4, ch5, ch6;
    static void fillChannels();
    static void collectCPPMcallback();

    //SignalMapping
    POSITIONCONTROL* p_con_address;
    int throttle_old;
    int throttle_difference;
    float mapf(int x, float in_min, float in_max, float out_min, float out_max);

  public:
    void begin(POSITIONCONTROL* pControllerPointer);
    void update(float RotorSignal[4]);
  };

  /*
    TODO: Maybe initialize STATIC variables. Has to be tested. (Compare to USR interrupt)
  */
/*==================================================================*/
  //Private (begin, start, stop)
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
    if(rcPulseTime/1000>=RC_START_BLOCKLENGTH) fillMode=RC_START;
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

/*==================================================================*/
  //Public functions
  void RADIOCONTROL::begin(POSITIONCONTROL* pControllerPointer){
      p_con_address = pControllerPointer;
      pinMode(RC_CPPM_PIN, INPUT);
      fillMode=RC_START;
      attachInterrupt(RC_CPPM_PIN,&RADIOCONTROL::collectCPPMcallback,CHANGE);
  }

  /*
  void RADIOCONTROL::update(float RotorSignal[4]){

    //TODO: Implement SignalMapping
    //Channel1 for Throttle
    //Channel2 for Pitch
    //Channel3 for Roll
    //Channel4 for JAW
    //Channel5 for TurnOff

    Map channel1 Input (1000-2000)
    Map channel2/channel3 Input (1000-2000) to degrees (-45°-45°)
    Map channel4 Input (1000-2000) to degrees (0°-360°)

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
    */


#endif
