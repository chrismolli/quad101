/*
Author: Christian Mollière
Combining Compass data with Curie IMU to get a 9 DOF IMU using a complementary filter
Revision 5 29.06.2016
*/

#ifndef IMU_H
#define IMU_H

/*==================================================================*/
  //Extern librarys
  #include "CurieIMU.h"
  #include "compass.h"
  #include "../params.h"

/*==================================================================*/
  //IMU constants
  #define INTMAX 32768        //16 bit Integer

/*==================================================================*/
  //Class definition
  class IMU{
    public:
      Compass com;
      float rot[3];
      float rot_vel[3];
      float accel[3];
      uint8_t rotationLimitExceeded;

      void begin();
      void update(float looptime);
      void debug(void);

    private:
      void initializeRollAndPitch(void);
      int raw_Accel[3];
      int raw_Gyro[3];
  };

/*==================================================================*/
  //Functions

void IMU::begin(){
  /*
   *  Initializes IMU and Compass
   */

  CurieIMU.begin();
  // verify connection
  /*Serial.println("Testing device connections...");
  if (CurieIMU.begin()) {
    Serial.println("CurieIMU connection successful");
  } else {
    Serial.println("CurieIMU connection failed");
  }
  */
  Serial.println("IMU calibration starts in 3 sec. Make sure your board is stable and upright");
  delay(3000);
  
  /*
  // The board must be resting in a horizontal position for the following calibration procedure to work correctly!
  Serial.print("Starting Gyroscope calibration and enabling offset compensation...");
  CurieIMU.autoCalibrateGyroOffset();
  Serial.println(" Done");

  Serial.print("Starting Acceleration calibration and enabling offset compensation...");
  //CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
  Serial.println(" Done");

  Serial.println("IMU sensor offsets after calibration...");  // Arduino1; Arduino2
  Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));      // 66.3; 124.8
  Serial.print("\t");
  Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));      // 46.8; 35.1
  Serial.print("\t"); // -2359
  Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));      // -54.6; -113.1
  Serial.print("\t"); // 1688
  Serial.print(CurieIMU.getGyroOffset(X_AXIS));               // 0.06; 2.68/2.74
  Serial.print("\t");
  Serial.print(CurieIMU.getGyroOffset(Y_AXIS));               // 0; -1.59
  Serial.print("\t");
  Serial.println(CurieIMU.getGyroOffset(Z_AXIS));             // -0.73, 0.12
  */

  CurieIMU.setAccelerometerOffset(X_AXIS, 124.8);
  CurieIMU.setAccelerometerOffset(Y_AXIS, 35.1);
  CurieIMU.setAccelerometerOffset(Z_AXIS, -113.1);

  CurieIMU.setGyroOffset(X_AXIS, 2.68);
  CurieIMU.setGyroOffset(Y_AXIS, -1.59);
  CurieIMU.setGyroOffset(Z_AXIS, 0.12);

  CurieIMU.setGyroRange(GYRORANGE);
  CurieIMU.setAccelerometerRange(ACCELRANGE);

  //PITCH and ROLL initialisation
  IMU::initializeRollAndPitch();

  //Compass and Jaw initialisation
  if(MAG_PLUGGED_IN){
    com.begin(GAIN_1_3,DECLINATION_ANGLE_DEGREE);
    com.readHeading();
    IMU::rot[2]=IMU::com.heading;
  }

  //variable for safety in case of flipping upside down
  rotationLimitExceeded = 0;
}

void IMU::update(float looptime){
  /*
   *  Updates all sensor values, computes rotation, stabilizes drifts
   */

  CurieIMU.readGyro(IMU::raw_Gyro[0],IMU::raw_Gyro[1],IMU::raw_Gyro[2]);
  CurieIMU.readAccelerometer(IMU::raw_Accel[0],IMU::raw_Accel[1],IMU::raw_Accel[2]);

  //Taking care of formatting acceleration
  accel[0] = -(float)raw_Accel[0]/INTMAX*ACCELRANGE*GRAVITY;
  accel[1] = (float)raw_Accel[1]/INTMAX*ACCELRANGE*GRAVITY;
  accel[2] = (float)raw_Accel[2]/INTMAX*ACCELRANGE*GRAVITY;

  //I.Calculating Angulars solo based on gyroscope's data
  //((float)looptime / 1000) converts SAMPLE_RATE into seconds
    IMU::rot_vel[0] = ((float)IMU::raw_Gyro[0] / INTMAX) * GYRORANGE ;
    IMU::rot_vel[1] = -((float)IMU::raw_Gyro[1] / INTMAX) * GYRORANGE ;
    IMU::rot_vel[2] = -((float)IMU::raw_Gyro[2] / INTMAX) * GYRORANGE ; //(same as Compass)

    IMU::rot[0] += IMU::rot_vel[0]*((float)looptime / 1000) * (180/PI);
    IMU::rot[1] += IMU::rot_vel[1]*((float)looptime / 1000) * (180/PI);
    IMU::rot[2] += IMU::rot_vel[2]*((float)looptime / 1000) * (180/PI);

    //Compensate sign reversing
    if(IMU::rot[0]<-180) IMU::rot[0]+=360;
    else if(IMU::rot[0]>180) IMU::rot[0]-=360.0;

    if(IMU::rot[1]<-180) IMU::rot[1]+=360;
    else if(IMU::rot[1]>180) IMU::rot[1]-=360.0;

    if(IMU::rot[2]<0.0) IMU::rot[2]+=360.0;
    else if(IMU::rot[2]>360.0) IMU::rot[2]-=360.0;

  //II. Compensate Gyrodrift with Acceldata if data !bullshit (complementary filtering)
  //int is 16bit at a range from -2g to 2g
    int lowpass = INTMAX / 4;
    int highpass = INTMAX;
    int accel_magnitude_approx = abs(IMU::raw_Accel[0]) +abs(IMU::raw_Accel[1]) +abs(IMU::raw_Accel[2]);

    if(    accel_magnitude_approx > lowpass
        && accel_magnitude_approx < highpass
        && abs(IMU::rot[0]) < 75
        && abs(IMU::rot[1]) < 75){
        //Turning around the X axis results in a vector on the Y-axis ROLL
        float acc_rot_x = atan2f((float)IMU::raw_Accel[1], (float)IMU::raw_Accel[2]) * 180 / PI;
        IMU::rot[0] = IMU_COMPLEMENTARY_WEIGHT * IMU::rot[0] + (1-IMU_COMPLEMENTARY_WEIGHT) * acc_rot_x;

        //Turning around the Y axis results in a vector on the X-axis PITCH
        float acc_rot_y = atan2f(-(float)IMU::raw_Accel[0], (float)IMU::raw_Accel[2]) * 180 / PI;
        IMU::rot[1] = IMU_COMPLEMENTARY_WEIGHT * IMU::rot[1] - (1-IMU_COMPLEMENTARY_WEIGHT) * acc_rot_y;

        //Compensate sign reversing
        if(IMU::rot[0]<-180) IMU::rot[0]+=360;
        else if(IMU::rot[0]>180) IMU::rot[0]-=360.0;

        if(IMU::rot[1]<-180) IMU::rot[1]+=360;
        else if(IMU::rot[1]>180) IMU::rot[1]-=360.0;
    }

  //III. Compensate JAW Gyro drift via Compass
  if(MAG_PLUGGED_IN){
      com.readTiltHeading(IMU::rot);
      if(abs(IMU::rot[0])<15.0 && abs(IMU::rot[1])<15.0 && IMU::rot[2] > 5.0 && IMU::rot[2] < 355.0){
        IMU::rot[2] = IMU_COMPLEMENTARY_WEIGHT * IMU::rot[2] + (1-IMU_COMPLEMENTARY_WEIGHT) * (IMU::com.heading);

        //Compensate sign reversing of JAW
        if(IMU::rot[2]<0.0) IMU::rot[2]+=360.0;
        else if(IMU::rot[2]>360.0) IMU::rot[2]-=360.0;
      }
  }

  if(abs(rot[0])>MAX_ROTATION || abs(rot[1])>MAX_ROTATION) rotationLimitExceeded = 1;
  else rotationLimitExceeded = 0;
}

void IMU::initializeRollAndPitch(void){
  CurieIMU.readAccelerometer(IMU::raw_Accel[0],IMU::raw_Accel[1],IMU::raw_Accel[2]);

  IMU::rot[0] = atan2f((float)IMU::raw_Accel[1], (float)IMU::raw_Accel[2]) * 180 / PI;
  IMU::rot[1] = atan2f(-(float)IMU::raw_Accel[0], (float)IMU::raw_Accel[2]) * 180 / PI;
  }

void IMU::debug(void){
  /*Serial.print(IMU::accel[0]);
  Serial.print(",");
  Serial.print(IMU::accel[1]);
  Serial.print(",");
  Serial.print(IMU::accel[2]);
  Serial.print(",");
  Serial.print(IMU::com.rawMag[0]);
  Serial.print(",");
  Serial.print(IMU::com.rawMag[1]);
  Serial.print(",");
  Serial.print(IMU::com.rawMag[2]);
  Serial.print(",");
  Serial.print(IMU::com.heading);
  Serial.print(",");*/
  Serial.print(IMU::rot[0]);
  Serial.print(",");
  Serial.print(IMU::rot[1]);
  Serial.print(",");
  Serial.print(IMU::rot[2]);
/*  Serial.print(",");
  Serial.print(IMU::raw_Gyro[0]);
  Serial.print(",");
  Serial.print(IMU::raw_Gyro[1]);
  Serial.print(",");
  Serial.print(IMU::raw_Gyro[2]);*/
  Serial.println(";");
}

#endif
