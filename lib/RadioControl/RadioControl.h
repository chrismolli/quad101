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
    static void fillChannels();
    static void collectCPPMcallback();

    //SignalMapping
    POSITIONCONTROL* positionController;
    int throttle_old;
    int throttle;
    float mapf(int x, float in_min, float in_max, float out_min, float out_max);

  public:
    void begin(POSITIONCONTROL* pControllerPointer);
    void update(float RotorSignal[4]);
    void debug();

    //Channels are filled with signal block length in microseconds
    static uint32_t ch1, ch2, ch3, ch4, ch5, ch6;
  };

  uint8_t RADIOCONTROL::fillMode=0;
  uint32_t RADIOCONTROL::pulseStart=0;
  uint32_t RADIOCONTROL::ch1=0;
  uint32_t RADIOCONTROL::ch2=0;
  uint32_t RADIOCONTROL::ch3=0;
  uint32_t RADIOCONTROL::ch4=0;
  uint32_t RADIOCONTROL::ch5=0;
  uint32_t RADIOCONTROL::ch6=0;
  uint32_t RADIOCONTROL::pulseEnd=0;
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

/*==================================================================*/
  //Public functions
  void RADIOCONTROL::begin(POSITIONCONTROL* pControllerPointer){
      positionController = pControllerPointer;
      pinMode(RC_CPPM_PIN, INPUT);
      RADIOCONTROL::fillMode=RC_START;
      attachInterrupt(RC_CPPM_PIN,&RADIOCONTROL::collectCPPMcallback,CHANGE);
      throttle=MIN_ROTOR_SIGNAL;
  }


  void RADIOCONTROL::update(float RotorSignal[4]){
      throttle_old = throttle;
      throttle = map(ch3, RC_MIN, RC_MAX, MIN_ROTOR_SIGNAL, MAX_ROTOR_SIGNAL);
      positionController->targetPosition[1] = mapf(ch2, RC_MIN, RC_MAX, -45, 45);
      positionController->targetPosition[0] = mapf(ch4, RC_MIN, RC_MAX, -45, 45);
      positionController->targetPosition[2] = mapf(ch1, RC_MIN, RC_MAX, -3, 3);

      int dif = throttle-throttle_old;
      RotorSignal[0] += dif;
      RotorSignal[1] += dif;
      RotorSignal[2] += dif;
      RotorSignal[3] += dif;
    }

  void RADIOCONTROL::debug(){
    Serial.println(ch5);
  };


#endif
