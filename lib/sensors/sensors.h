#ifndef SENSORS_H
#define SENSORS_H

/* Super header for collection all the sensor libs
*/

/*==================================================================*/
  //Sensor libraries
  #include "IMU/imu.h"
  #include "BMP180/bmp.h"
  #include "../params.h"

/*==================================================================*/
  //Class definition
  class SENSORS{
    public:
      IMU imu;  //includes compass
      BMP180 bmp;
      void begin(void);
      void getActive(void);
      void update(void);

    private:
      uint8_t number_of_active_sensors;
  };

/*==================================================================*/
  //Functions
  void SENSORS::begin(void){
    imu.begin();
    if(BMP_PLUGGED_IN) bmp.begin();
  }

  void SENSORS::update(void){
    imu.update(SAMPLE_RATE);
    bmp.update();
  }

  void SENSORS::getActive(void){
    number_of_active_sensors = 1;
    if(MAG_PLUGGED_IN) number_of_active_sensors++;
    if(BMP_PLUGGED_IN) number_of_active_sensors++;

    if(Serial){
        Serial.print(number_of_active_sensors);
        Serial.println(" active sensors found!");
    }
  }



#endif
