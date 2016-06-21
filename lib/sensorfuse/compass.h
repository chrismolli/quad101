/*
Author: Christian Mollière
Magnetometer library for Honeywell HMC5883L.
Revision 4, 21.06.2016

Uses EEPROM registers from 0x00 to (including) 0x05!
*/
#ifndef COMPASS_H
#define COMPASS_H

/*==================================================================*/
  //Extern librarys
  #include "Wire.h"
  #include "CurieEEPROM.h"

/*==================================================================*/
  //I2C sensor main address
  #define HMC5883_ADDRESS                         0x1E     // 0011110b

  //EEPROM CONSTANTS
  #define X_OFF_EEPROM_ADDR                       0x00
  #define Y_OFF_EEPROM_ADDR                       0x02
  #define Z_OFF_EEPROM_ADDR                       0x04

/*==================================================================*/
  //Sensor data addresses
  typedef enum
  {
    CONFIG_REG_A                                = 0x00,
    CONFIG_REG_B                                = 0x01,
    MODE_REG                                    = 0x02,
    X_MSB_REG                                   = 0x03,
    X_LSB_REG                                   = 0x04,
    Z_MSB_REG                                   = 0x05,
    Z_LSB_REG                                   = 0x06,
    Y_MSB_REG                                   = 0x07,
    Y_LSB_REG                                   = 0x08,
    STATUS_REG                                  = 0x09,
    IDENTIFICATION_REG_A                        = 0x0A,
    IDENTIFICATION_REG_B                        = 0x0B,
    IDENTIFICATION_REG_C                        = 0x0C,
    TEMP_OUT_H_REG                              = 0x31,
    TEMP_OUT_L_REG                              = 0x32
  } MagRegister_t;

/*==================================================================*/
  //Sensor gain settings
  typedef enum
  {
    GAIN_1_3                        = 0x20,  // +/- 1.3
    GAIN_1_9                        = 0x40,  // +/- 1.9
    GAIN_2_5                        = 0x60,  // +/- 2.5
    GAIN_4_0                        = 0x80,  // +/- 4.0
    GAIN_4_7                        = 0xA0,  // +/- 4.7
    GAIN_5_6                        = 0xC0,  // +/- 5.6
    GAIN_8_1                        = 0xE0   // +/- 8.1
  } sensorGain_t;

/*==================================================================*/
//Class
class Compass{
  public:
    short offset[3];            //raw
    short rawMag[3];            //raw
    float heading;              //degree
    float declinationAngle;     //radian

    void  write(MagRegister_t reg, unsigned char hex);
    void  read(boolean calibrationMode);
    void  initialize(sensorGain_t gain,float declinationDegree);
    void  readHeading(void);
    void  readTiltHeading(float rot[3]);
    void  sendSerial(void);
    void  calibrate(void);

  private:
    float fixHeading;           //degree
};

/*==================================================================*/

//Functions
void Compass::write(MagRegister_t reg, unsigned char hex){
    Wire.beginTransmission(HMC5883_ADDRESS);
    Wire.write(reg);
    Wire.write(hex);
    Wire.endTransmission();
}


void Compass::read(boolean calibrationMode = FALSE){
  //Set pointer to 4th Register
  Wire.beginTransmission(HMC5883_ADDRESS);
  Wire.write(0x03);
  Wire.endTransmission();

  //Send data request of 6 bytes
  Wire.requestFrom(HMC5883_ADDRESS,6);

  //Read raw magnetometer values
  if(Wire.available() >= 6 && !calibrationMode){
    //Read X
    Compass::rawMag[0]=(Wire.read()<<8 | Wire.read()) - Compass::offset[0];         //Read upper 8 bits (MSB)
    //Read Z
    Compass::rawMag[2]=(Wire.read()<<8 | Wire.read()) - Compass::offset[1];
    //Read Y
    Compass::rawMag[1]=(Wire.read()<<8 | Wire.read()) - Compass::offset[2];
  }else if(Wire.available() >= 6){
    //No offset subtraction in calibration mode
    //Read X
    Compass::rawMag[0]=(Wire.read()<<8 | Wire.read()) ;
    //Read Z
    Compass::rawMag[2]=(Wire.read()<<8 | Wire.read()) ;
    //Read Y
    Compass::rawMag[1]=(Wire.read()<<8 | Wire.read()) ;
  }
}


void Compass::initialize(sensorGain_t gain,float declinationDegree){
  if(Serial){
    Serial.println("Initializing Magnetometer...");
  }

  delay(6);
  Wire.begin(); // Initializing I2C connection

  //Send config to sensor
  write(CONFIG_REG_B,gain); // Set gain
  write(MODE_REG,0x00);     // 0x00 Continous Mode, 0x01 Single Mode
  write(CONFIG_REG_A,0x78); // 0x78 8-average 75Hz, 0x18 1-average 75Hz, 0x74 8-average 30Hz
  delay(6);

  //Memset
  Compass::rawMag[0]=0;
  Compass::rawMag[1]=0;
  Compass::rawMag[2]=0;
  Compass::heading=0;
  Compass::declinationAngle=declinationDegree/180*PI;

  //Read hardiron offset from EEPROM
  Compass::offset[0]=EEPROM.read(X_OFF_EEPROM_ADDR) << 8 | EEPROM.read(X_OFF_EEPROM_ADDR+1);
  Compass::offset[1]=EEPROM.read(Y_OFF_EEPROM_ADDR) << 8 | EEPROM.read(Y_OFF_EEPROM_ADDR+1);
  Compass::offset[2]=EEPROM.read(Z_OFF_EEPROM_ADDR) << 8 | EEPROM.read(Z_OFF_EEPROM_ADDR+1);

  //Skip first readings and set fixpoint
  Compass::read();
  delay(30);

  Compass::readHeading();
  Compass::fixHeading=Compass::heading;

  if(Serial){
    Serial.println("  Done");
  }
}


void Compass::readHeading(){
  Compass::read();
  //Calculate heading
  Compass::heading=atan2f((float)Compass::rawMag[1],(float)Compass::rawMag[0])+declinationAngle;
  //Compensate sign reversing
  if(Compass::heading<0) Compass::heading +=2*PI;
  else if(Compass::heading>2*PI) Compass::heading -=2*PI;
  //Convert heading to degrees
  Compass::heading *=180/PI;
}


void Compass::readTiltHeading(float rot[3]){
  /*This function needs to know the current roll and pitch of the sensor!
  */

  //Read raw Magnetometer values
  Compass::read();

  //rot[0]:=Roll theta x, rot[1]:=Pitch phi y
  float roll=rot[0]; // +0
  float pitch=rot[1]; // +0
  //Precompute sin cos
  float sinRoll=sin(roll/180*PI);
  float cosRoll=cos(roll/180*PI);
  float sinPitch=sin(pitch/180*PI);
  float cosPitch=cos(pitch/180*PI);

  //Calculate tilt compensated magnetometer values
  float y_comp=(float)Compass::rawMag[1]*sinRoll*sinPitch + (float)Compass::rawMag[1]*cosRoll - (float)Compass::rawMag[2]*sinRoll;
  float x_comp=(float)Compass::rawMag[0]*cosPitch + (float)Compass::rawMag[1]*sinRoll*sinPitch + (float)Compass::rawMag[2]*cosRoll*sinPitch;

  //Calculate new heading
  Compass::heading=atan2f(y_comp,x_comp);
  /*
  if(x_comp>0 && y_comp>0)        Compass::heading=atan2f(y_comp,x_comp);
  else if(x_comp<0)               Compass::heading=PI+atan2f(y_comp,x_comp);
  else if(x_comp>0 && y_comp<=0)  Compass::heading=2*PI+atan2f(y_comp,x_comp);
  else if(x_comp==0)              Compass::heading=PI/2;
  */

  //Add magnetic declination
  Compass::heading+=declinationAngle;

  //Compensate sign reversing
  if(Compass::heading<0) Compass::heading+=2*PI;
  else if(Compass::heading>2*PI) Compass::heading-=2*PI;

  //Convert heading to degrees
  Compass::heading*=180/PI;
}


void Compass::calibrate(void){
  //I. Calibration data
  int calibrationPoints = 1000;
  short xMin=0,xMax=0,yMin=0,yMax=0,zMin=0,zMax=0;

  //II. User prompt start
  Serial.println("******************************************************************");
  Serial.println("Magnetometer calibration mode active...");
  Serial.println("******************************************************************");
  Serial.println("Please move magnetometer in full circles until calibration is done");
  Serial.println("Program will read 1000 datapoints in 30 seconds");
  Serial.println("Press any key to continue...");
  while(!Serial.available());

  //III. Collecting data
  for(int i=0; i<calibrationPoints; i++){
    Compass::read(TRUE);

    if(xMin>Compass::rawMag[0]) xMin=Compass::rawMag[0];
    if(xMax<Compass::rawMag[0]) xMax=Compass::rawMag[0];

    if(yMin>Compass::rawMag[1]) yMin=Compass::rawMag[1];
    if(yMax<Compass::rawMag[1]) yMax=Compass::rawMag[1];

    if(zMin>Compass::rawMag[2]) zMin=Compass::rawMag[2];
    if(zMax<Compass::rawMag[2]) zMax=Compass::rawMag[2];

    Serial.println(i+1);

    // 1000 * 30ms = 30 seconds
    delay(30);
  }Serial.println("  Done");

  //IV. Calculate hardiron offsets
  Compass::offset[0]=(short)((xMax-xMin)/2);
  Compass::offset[1]=(short)((yMax-yMin)/2);
  Compass::offset[2]=(short)((zMax-zMin)/2);

  /*if(xMax<0 && xMin<0) Compass::offset[0]-= xMax;
  else if(xMax>0 && xMin>0) Compass::offset[0]-= xMin;

  if(yMax<0 && yMin<0) Compass::offset[1]-= yMax;
  else if(yMax>0 && yMin>0) Compass::offset[1]-= yMin;

  if(zMax<0 && zMin<0) Compass::offset[2]-= zMax;
  else if(zMax>0 && zMin>0) Compass::offset[2]-= zMin;*/

  //V. Display offsets
  Serial.println("Hardiron offsets found...");
  Serial.print("Offset X: ");
  Serial.print(Compass::offset[0]);
  Serial.print(" Offset Y: ");
  Serial.print(Compass::offset[1]);
  Serial.print(" Offset Z: ");
  Serial.print(Compass::offset[2]);
  Serial.println();

  //VI. Saving offsets to EEPROM
  Serial.println("Saving hardiron offsets...");
  EEPROM.clear();

  EEPROM.write(X_OFF_EEPROM_ADDR+1,Compass::offset[0] & 0xFF); // Write X LSB
  EEPROM.write(X_OFF_EEPROM_ADDR,Compass::offset[0] >> 8);             // Write X MSB

  EEPROM.write(Y_OFF_EEPROM_ADDR+1,Compass::offset[1] & 0xFF); // Write Y LSB
  EEPROM.write(Y_OFF_EEPROM_ADDR,Compass::offset[1] >> 8);             // Write Y MSB

  EEPROM.write(Z_OFF_EEPROM_ADDR+1,Compass::offset[2] & 0xFF);         // Write Z LSB
  EEPROM.write(Z_OFF_EEPROM_ADDR,Compass::offset[2] >> 8);             // Write Z MSB

  //VII. Check EEPROM
  uint8_t writeCheck = 0;
  writeCheck += ( (EEPROM.read(X_OFF_EEPROM_ADDR) << 8 | EEPROM.read(X_OFF_EEPROM_ADDR+1)) && Compass::offset[0]);
  writeCheck += ( (EEPROM.read(Y_OFF_EEPROM_ADDR) << 8 | EEPROM.read(Y_OFF_EEPROM_ADDR+1)) && Compass::offset[1]);
  writeCheck += ( (EEPROM.read(Z_OFF_EEPROM_ADDR) << 8 | EEPROM.read(Z_OFF_EEPROM_ADDR+1)) && Compass::offset[2]);

  if(writeCheck==3) Serial.println("SUCCESS: Offsets written to EEPROM!");
  else              Serial.println("ERROR: Could not write offsets to EEPROM!");
}


void Compass::sendSerial(void){
    //Prints the time and angle data to serial
    Serial.print("Fixed Heading ");
    Serial.print(Compass::fixHeading);
    Serial.print(" Tilt Compensated Heading ");
    Serial.print(Compass::heading);
    Serial.print(" Bias ");
    Serial.print(Compass::heading-Compass::fixHeading);
    Serial.println();
}

#endif
