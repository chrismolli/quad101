/*
Author: Christian Mollière
Combining Compass data with Curie IMU to get a 9 DOF IMU using a complementary filter
Revision 4 11.06.2016
*/

#ifndef SENSORFUSE_H
#define SENSORFUSE_H

/*==================================================================*/
  //Extern librarys
  #include "CurieIMU.h"
  #include "compass.h"
  #include "../params.h"

/*==================================================================*/
  //IMU constants
  //#define GYRORANGE 2
  #define INTMAX 32768        //16 bit Integer
  //#define COMPLEMENTARY_WEIGHT 0.98

  //COMPASS contants
  //#define MAG_X_OFFSET 8.7539       //From calibration tool
  //#define MAG_Y_OFFSET -442.0650    //From calibration tool
  //#define MAG_Z_OFFSET -52.7955     //From calibration tool
  //#define DECLINATION_ANGLE_RADIAN 0

/*==================================================================*/
  //Class definition
  class IMU{
    public:
      Compass com;
      float rot[3];
      float rot_vel[3];
      void begin();
      void update(float looptime);
      void sendSerial(void);
      void debug(void);

    private:
      int raw_Accel[3];
      int raw_Gyro[3];
  };

/*==================================================================*/
  //Functions

void IMU::begin(){
  //Initialize IMU
  CurieIMU.begin();
  // verify connection
  Serial.println("Testing device connections...");
  if (CurieIMU.begin()) {
    Serial.println("CurieIMU connection successful");
  } else {
    Serial.println("CurieIMU connection failed");
  }

  Serial.println("Initializing calibration...");

  Serial.println("Calibration starts in 5 sec. Make sure your board is stable and upright");
  delay(5000);

  // The board must be resting in a horizontal position for
  // the following calibration procedure to work correctly!
  Serial.print("Starting Gyroscope calibration and enabling offset compensation...");
  CurieIMU.autoCalibrateGyroOffset();
  Serial.println(" Done");

  Serial.print("Starting Acceleration calibration and enabling offset compensation...");
  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
  Serial.println(" Done");

  Serial.println("Internal sensor offsets AFTER calibration...");
  Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
  Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
  Serial.print("\t"); // -2359
  Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));
  Serial.print("\t"); // 1688
  Serial.print(CurieIMU.getGyroOffset(X_AXIS));
  Serial.print("\t"); // 0
  Serial.print(CurieIMU.getGyroOffset(Y_AXIS));
  Serial.print("\t"); // 0
  Serial.println(CurieIMU.getGyroOffset(Z_AXIS));

  CurieIMU.setGyroRange(GYRORANGE);

  //Compass initialisation
  com.initialize(GAIN_1_3,DECLINATION_ANGLE_RADIAN);
}

void IMU::update(float looptime){

  CurieIMU.readGyro(IMU::raw_Gyro[0],IMU::raw_Gyro[1],IMU::raw_Gyro[2]);
  CurieIMU.readAccelerometer(IMU::raw_Accel[0],IMU::raw_Accel[1],IMU::raw_Accel[2]);

  //I.Calculating Angulars solo based on gyroscope's data
  //((float)looptime / 1000) converts SAMPLE_RATE into seconds
    IMU::rot_vel[0] = ((float)IMU::raw_Gyro[0] / INTMAX) * GYRORANGE ;
    IMU::rot_vel[1] = ((float)IMU::raw_Gyro[1] / INTMAX) * GYRORANGE ;
    IMU::rot_vel[2] = ((float)IMU::raw_Gyro[2] / INTMAX) * GYRORANGE ;

    IMU::rot[0] += IMU::rot_vel[0]*((float)looptime / 1000) * (180/PI);
    IMU::rot[1] += IMU::rot_vel[1]*((float)looptime / 1000) * (180/PI);
    IMU::rot[2] += IMU::rot_vel[2]*((float)looptime / 1000) * (180/PI);

  //II. Compensate Gyrodrift with Acceldata if data !bullshit (complementary filtering)
  //int is 16bit at a range from -2g to 2g
    int lowpass = INTMAX / 4;
    int highpass = INTMAX;
    int accel_magnitude_approx = abs(IMU::raw_Accel[0]) +abs(IMU::raw_Accel[1]) +abs(IMU::raw_Accel[2]);
    if(accel_magnitude_approx > lowpass && accel_magnitude_approx < highpass){
        //Turning around the X axis results in a vector on the Y-axis PITCH
        float acc_rot_x = atan2f((float)IMU::raw_Accel[1], (float)IMU::raw_Accel[2]) * 180 / PI;
        IMU::rot[0] = COMPLEMENTARY_WEIGHT * IMU::rot[0] + (1-COMPLEMENTARY_WEIGHT) * acc_rot_x;

        //Turning around the Y axis results in a vector on the X-axis ROLL
        float acc_rot_y = atan2f((float)IMU::raw_Accel[0], (float)IMU::raw_Accel[2]) * 180 / PI;
        IMU::rot[1] = COMPLEMENTARY_WEIGHT * IMU::rot[1] - (1-COMPLEMENTARY_WEIGHT) * acc_rot_y;
    }

  //III. Compensate JAW Gyro drift via Compass
    com.readTiltHeading(IMU::rot);
    /*if(IMU::rot[0]<3.0 && IMU::rot[1]<3.0){
      IMU::rot[2] = COMPLEMENTARY_WEIGHT * IMU::rot[2] + (1-COMPLEMENTARY_WEIGHT) * (IMU::com.heading-IMU::com.fixHeading);
    }
    //Compensate sign reversing of JAW
    if(IMU::rot[2]<0.0) IMU::rot[2]+=360.0;
    else if(IMU::rot[2]>360.0) IMU::rot[2]-=360.0;*/

}

void IMU::sendSerial(void){
    //Prints the time and angle data to serial
    //Serial.print("Time ");
    //Serial.print(millis());
    Serial.print(" Roll ");
    Serial.print(IMU::rot[0]);
    Serial.print(" Pitch ");
    Serial.print(IMU::rot[1]);
    Serial.print(" Jaw ");
    Serial.print(IMU::rot[2]);
    Serial.println();
}

void IMU::debug(void){
  Serial.print(IMU::raw_Accel[0]);
  Serial.print(",");
  Serial.print(IMU::raw_Accel[1]);
  Serial.print(",");
  Serial.print(IMU::raw_Accel[2]);
  Serial.print(",");
  Serial.print(IMU::com.rawMag[0]);
  Serial.print(",");
  Serial.print(IMU::com.rawMag[1]);
  Serial.print(",");
  Serial.print(IMU::com.rawMag[2]);
  Serial.print(",");
  Serial.print(IMU::rot[0]);
  Serial.print(",");
  Serial.print(IMU::rot[1]);
  Serial.print(",");
  Serial.print(IMU::rot[2]);
  Serial.print(",");
  Serial.print(IMU::raw_Gyro[0]);
  Serial.print(",");
  Serial.print(IMU::raw_Gyro[1]);
  Serial.print(",");
  Serial.print(IMU::raw_Gyro[2]);
  Serial.println(";");
}

#endif
