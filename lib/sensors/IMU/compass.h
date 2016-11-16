/*
Author: Christian Mollière
Magnetometer library for Honeywell HMC5883L.
Revision 5, 29.06.2016

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
  //Classdefinition
class Compass{
  public:
    short offset[3];            //raw
    short rawMag[3];            //raw
    float heading;              //degree

    void  write(MagRegister_t reg, unsigned char hex);
    void  read(void);
    void  begin(sensorGain_t gain,float magneticDeclination);
    void  readHeading(void);
    void  readTiltHeading(float rot[3]);
    void  calibrate(void);

  private:
    float declination;          //radian
};

/*==================================================================*/
  //Functions

void Compass::write(MagRegister_t reg, unsigned char hex){
  /*
   *  Writes I2C message to magnetometer
   */
    Wire.beginTransmission(HMC5883_ADDRESS);
    Wire.write(reg);
    Wire.write(hex);
    Wire.endTransmission();
}

void Compass::read(void){
  /*
   *  Reads magnetometer registers
   */

  //Set pointer to 4th Register
  Wire.beginTransmission(HMC5883_ADDRESS);
  Wire.write(0x03);
  Wire.endTransmission();

  //Send data request of 6 bytes
  Wire.requestFrom(HMC5883_ADDRESS,6);

  //Read raw magnetometer values
  //Read X
  Compass::rawMag[0]= (Wire.read()<<8 | Wire.read()) - Compass::offset[0];
  //Read Z
  Compass::rawMag[2]= (Wire.read()<<8 | Wire.read()) - Compass::offset[2];
  //Read Y
  Compass::rawMag[1]= (Wire.read()<<8 | Wire.read()) - Compass::offset[1];
}

void Compass::begin(sensorGain_t gain,float magneticDeclinationDegree){
  /*
   *  Initializes magnetometer
   */

  if(Serial){
    Serial.println("Initializing Magnetometer...");
  }

  // Initializing I2C connection
  Wire.begin();

  //Send config to sensor
  Compass::write(CONFIG_REG_B,gain); // Set gain
  Compass::write(MODE_REG,0x00);     // 0x00 Continous Mode, 0x01 Single Mode
  Compass::write(CONFIG_REG_A,0x78); // 0x78 8-average 75Hz, 0x18 1-average 75Hz, 0x74 8-average 30Hz
  delay(6);

  //Memset
  Compass::rawMag[0]=0;
  Compass::rawMag[1]=0;
  Compass::rawMag[2]=0;
  Compass::heading=0;
  Compass::declination=magneticDeclinationDegree/180*PI;

  //Read hardiron offset from EEPROM
  for(int i=0; i<6; i+=2){
    Compass::offset[i/2]=EEPROM.read(X_OFF_EEPROM_ADDR+i) << 8 | EEPROM.read(X_OFF_EEPROM_ADDR+i+1);
  }

  //Print offsets
  Serial.println("Magnetic offsets read from EEPROM!");
  Serial.print("Offset X: ");
  Serial.print(Compass::offset[0]);
  Serial.print(" Offset Y: ");
  Serial.print(Compass::offset[1]);
  Serial.print(" Offset Z: ");
  Serial.print(Compass::offset[2]);
  Serial.println();

  //Skip first readings
  Compass::read();
  delay(2000);

  if(Serial){
    Serial.println("  Done");
  }
}

void Compass::readHeading(){
  /*
   *  Reads uncompensated heading
   */
  Compass::read();
  //Calculate heading
  Compass::heading=atan2f(-(float)Compass::rawMag[0],(float)Compass::rawMag[1])+declination;
  //Compensate sign reversing
  if(Compass::heading<0) Compass::heading +=2*PI;
  else if(Compass::heading>2*PI) Compass::heading -=2*PI;
  //Convert heading to degrees
  Compass::heading *=180/PI;
}

void Compass::readTiltHeading(float rot[3]){
  /*
   *  Compensates magnetometer readings from tilt
   *  This function needs to know the current roll and pitch of the sensor!
   */

  Compass::read();

  //Precompute sin cos
  float roll = rot[0];
  float pitch = -rot[1];
  float sinRoll=sin(roll/180*PI);
  float cosRoll=cos(roll/180*PI);
  float sinPitch=sin(pitch/180*PI);
  float cosPitch=cos(pitch/180*PI);

  //Calculate tilt compensated magnetometer values
  float x_comp=(float)Compass::rawMag[1]*cosPitch - (float)Compass::rawMag[0]*sinRoll*sinPitch + (float)Compass::rawMag[2]*cosRoll*sinPitch;
  float y_comp=(-1)*(float)Compass::rawMag[0]*cosRoll - (float)Compass::rawMag[2]*sinRoll;

  //Calculate new heading
  Compass::heading=atan2f(y_comp,x_comp);

  //Add magnetic declination
  Compass::heading+=declination;

  //Compensate sign reversing
  if(Compass::heading<0) Compass::heading+=2*PI;
  else if(Compass::heading>2*PI) Compass::heading-=2*PI;

  //Convert heading to degrees
  Compass::heading*=180/PI;
}

void Compass::calibrate(void){
  /*
   *  Calibrates the hardiron offsets of the magnetometer
   *  Serial connection needed!
   */
  //I. Calibration data
  int calibrationPoints = 2000;
  short xMin=0,xMax=0,yMin=0,yMax=0,zMin=0,zMax=0;
  // Reseting offsets and scales
  Compass::offset[0]=0;
  Compass::offset[1]=0;
  Compass::offset[2]=0;

  //II. User prompt start
  Serial.println("******************************************************************");
  Serial.println("Magnetometer calibration mode active...");
  Serial.println("******************************************************************");
  Serial.println("Please move magnetometer in full circles until calibration is done");
  Serial.println("Program will read 2000 datapoints in 60 seconds");
  Serial.println("Press any key to continue...");
  while(!Serial.available());
  Serial.println("Start...");
  //III. Collecting data
  for(int i=0; i<calibrationPoints; i++){
    Compass::read();

    if(xMin > Compass::rawMag[0]) xMin=Compass::rawMag[0];
    if(xMax < Compass::rawMag[0]) xMax=Compass::rawMag[0];

    if(yMin > Compass::rawMag[1]) yMin=Compass::rawMag[1];
    if(yMax < Compass::rawMag[1]) yMax=Compass::rawMag[1];

    if(zMin > Compass::rawMag[2]) zMin=Compass::rawMag[2];
    if(zMax < Compass::rawMag[2]) zMax=Compass::rawMag[2];

    Serial.print(i+1);
    Serial.print(",");
    Serial.print(rawMag[0]);
    Serial.print(",");
    Serial.print(rawMag[1]);
    Serial.print(",");
    Serial.print(rawMag[2]);
    Serial.println(";");
    delay(30);

  }Serial.println("  Done");

  //IV. Calculate hardiron offsets and scaling
  Compass::offset[0]=round((xMax+xMin)/2);
  Compass::offset[1]=round((yMax+yMin)/2);
  Compass::offset[2]=round((zMax+zMin)/2);

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

  EEPROM.write(X_OFF_EEPROM_ADDR,   Compass::offset[0] >> 8);           // Write X MSB
  EEPROM.write(X_OFF_EEPROM_ADDR+1, Compass::offset[0] & 0xFF);         // Write X LSB

  EEPROM.write(Y_OFF_EEPROM_ADDR,   Compass::offset[1] >> 8);           // Write Y MSB
  EEPROM.write(Y_OFF_EEPROM_ADDR+1, Compass::offset[1] & 0xFF);         // Write Y LSB

  EEPROM.write(Z_OFF_EEPROM_ADDR,   Compass::offset[2] >> 8);           // Write Z MSB
  EEPROM.write(Z_OFF_EEPROM_ADDR+1, Compass::offset[2] & 0xFF);         // Write Z LSB

  //VII. Check EEPROM
  uint8_t writeCheck = 0;
  for(int i=0;i<6;i+=2){
    writeCheck += ( (EEPROM.read(X_OFF_EEPROM_ADDR+i) << 8 | EEPROM.read(X_OFF_EEPROM_ADDR+i+1)) == (unsigned)Compass::offset[i/2]);
  }

  if(writeCheck==3) Serial.println("SUCCESS: Offsets written to EEPROM!");
  else              Serial.println("ERROR: Could not write offsets to EEPROM!");
}

#endif
