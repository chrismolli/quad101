#ifndef USR_H
#define USR_H

/*
  Lib for the HC-SRF04 Ultrasonic Rangefinder.
  Used for shortrange height measurements.
*/

/*==================================================================*/
  //Sensor libraries
  #include <Arduino.h>
  #include "../params.h"

/*==================================================================*/
  #ifndef USR_TRIGGER_PIN
    #define USR_TRIGGER_PIN 13
  #endif

  #ifndef USR_ECHO_PIN
    #define USR_ECHO_PIN 12
  #endif

/*==================================================================*/
  //Class definition
  class USR{
    public:
      void begin();
      void pulseOut();
      static void pulseIn();

      static float height;

    private:
      static uint8_t pulse_flag;
      static unsigned long pulse_time;
      static unsigned long echo_start;
      static unsigned long echo_end;

  };

  //Definition of the static variables is needed for the interrupt
  float USR::height=0;
  uint8_t USR::pulse_flag=0;
  unsigned long USR::pulse_time=0;
  unsigned long USR::echo_start=0;
  unsigned long USR::echo_end=0;

/*==================================================================*/
  //Functions
  void USR::begin(){
    pinMode(USR_TRIGGER_PIN,OUTPUT);
    pinMode(USR_ECHO_PIN,INPUT);
    digitalWrite(USR_TRIGGER_PIN,LOW);
    pulse_flag=0;
    delay(15);
    //Attach itself as an system interrupt
    attachInterrupt(USR_ECHO_PIN,&USR::pulseIn,CHANGE);
  }

  void USR::pulseOut(){
    if(!pulse_flag){
      digitalWrite(USR_TRIGGER_PIN,HIGH);
      delayMicroseconds(11);
      digitalWrite(USR_TRIGGER_PIN,LOW);
      pulse_time=micros();
      pulse_flag=1;
    }else if((micros()-pulse_time)>2000){
      pulse_flag=0;
    }
  }


  void USR::pulseIn(){
    if(pulse_flag){
      switch (digitalRead(USR_ECHO_PIN)) {
        case HIGH:
          echo_start=micros();
          break;
        case LOW:
          echo_end=micros();
          height=(float)(echo_end-echo_start)/10000*340/2;
          Serial.print(height);
          Serial.println(" cm");
          pulse_flag=0;
          break;
      }
    }
  }


  #endif
