#ifndef SENSORS_H
#define SENSORS_H

/* Super header for collection all the sensor libs
*/

/*==================================================================*/
  //Sensor libraries
  #include <CurieBLE.h>
  #include "../params.h"
  #include "IMU/imu.h"
  #include "BMP180/bmp.h"
  #include "USR/usr.h"
  #include "ADXL335/ADXL335.h"
  #include "GPS/SKM53.h"
  #include "SoftwareSerial.h"


/*==================================================================*/
  //Class definition
  class SENSORS{
    public:
      IMU imu;  //includes compass
      BMP180 bmp;
      USR usr;
      SKM53 skm53;
      ADXL335 adxl335;
      void begin(SoftwareSerial* gpsSerial);
      void getActive(void);
      void update(void);
      void updateSlow(void);
  };

/*==================================================================*/
  //Functions
  void SENSORS::begin(SoftwareSerial* skm53Serial){
    imu.begin();
    if(BMP_PLUGGED_IN) bmp.begin();
    if(USR_PLUGGED_IN) usr.begin();
    if(SKM53_PLUGGED_IN) skm53.begin(skm53Serial);
    if(ADXL335_PLUGGED_IN) adxl335.begin();
  }

  void SENSORS::update(void){
    imu.update(SAMPLE_RATE);
    if(BMP_PLUGGED_IN) bmp.update();
    if(ADXL335_PLUGGED_IN) adxl335.update();
  }

  void SENSORS::updateSlow(void){
    if(USR_PLUGGED_IN) usr.pulseOut();
    if(SKM53_PLUGGED_IN) skm53.update();
  }

#endif
