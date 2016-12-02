#ifndef ADXL335_H
#define ADXL335_H

/* ADXL335 is a accelerometer sensor. it might be used for redundancy and filtering the main IMU
*/

/*==================================================================*/
  //Extern libraries
#include "Arduino.h"
#include "../../params.h"

/*==================================================================*/
  //Classdefinition
  class ADXL335{
  private:
    const static int xpin = XPIN;
    const static int ypin = YPIN;
    const static int zpin = ZPIN;

    int zero_Gx;
    int zero_Gy;
    int zero_Gz;

    float scale;

  public:
    float accel[3];

    void begin(void);
    void update(void);
    void debug(void);
  };

/*==================================================================*/
  //Public Functions
  void ADXL335::begin(void){
    zero_Gx = analogRead(xpin); //x_Offset (331.5 oder 496);
    zero_Gy = analogRead(ypin); //y_Offset (329.5 oder 492)
    zero_Gz = analogRead(zpin)-98.1; //z_Offset (340 oder 498)
    scale = 98.1; //combines range with sensitivity 1.45*67.6

    //supply voltage 5v ADXL335 Sensitivity is 330mv/g; 330 * 676/3.3/1000

    //float zero_G = 512.0; //ADC is 0~1023  the zero_g should be in the middle
    //float zero_G = 338.0; //ADXL335 power supply by Vs 3.3V: 3.3V/5V*1024=676/2=338,by default is 338
  }


  void ADXL335::update(void){
    accel[0] = (float)analogRead(xpin);  //read from xpin
    delay(1); //
    accel[1] = (float)analogRead(ypin);  //read from ypin
    delay(1);
    accel[2] = (float)analogRead(zpin);  //read from zpin

    accel[0] = -(accel[0]-zero_Gx)/scale*GRAVITY;
    accel[1] = (accel[1]-zero_Gy)/scale*GRAVITY;
    accel[2] = (accel[2]-zero_Gz)/scale*GRAVITY;
}

void ADXL335::debug(void){
  if (Serial){
  Serial.print(accel[0]);
  Serial.print(",");
  Serial.print(accel[1]);
  Serial.print(",");
  Serial.print(accel[2]);
  Serial.println(";");
}
}

#endif
