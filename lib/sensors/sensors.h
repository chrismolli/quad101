#ifndef SENSORS_H
#define SENSORS_H

/* Super header for collection all the sensor libs
*/

/*==================================================================*/
  //Sensor libraries
  #include "IMU/imu.h"
  #include "BMP180/bmp.h"
  #include "USR/usr.h"
  #include <CurieBLE.h>
  #include "../params.h"

/*==================================================================*/
  //Class definition
  class SENSORS{
    public:
      IMU imu;  //includes compass
      BMP180 bmp;
      USR usr;
      void begin(void);
      void getActive(void);
      void update(void);
      void updateSlow(void);
  };

/*==================================================================*/
  //Functions
  void SENSORS::begin(void){
    imu.begin();
    if(BMP_PLUGGED_IN) bmp.begin();
    if(USR_PLUGGED_IN) usr.begin();
  }

  void SENSORS::update(void){
    imu.update(SAMPLE_RATE);
    if(BMP_PLUGGED_IN) bmp.update();
  }

  void SENSORS::updateSlow(void){
    if(USR_PLUGGED_IN) usr.pulseOut();
  }

#endif
