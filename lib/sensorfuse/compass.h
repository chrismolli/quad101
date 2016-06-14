/*
Author: Christian Mollière
Magnetometer library for Honeywell HMC5883L.
Revision 3, 11.06.2016
*/
#ifndef COMPASS_H
#define COMPASS_H

/*==================================================================*/
  //Extern librarys
  #include "Wire.h"

/*==================================================================*/
  //I2C sensor main address
  #define HMC5883_ADDRESS                         0x1E     // 0011110b

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
    void  read(void);
    void  initialize(sensorGain_t gain,short offsetX,short offsetY,short offsetZ,float declinationDegree);
    void  readHeading(void);
    void  readTiltHeading(float rot[3]);
    void  debug(void);
    void sendSerial(void);

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

void Compass::read(void){
  //Set pointer to 4th Register
  Wire.beginTransmission(HMC5883_ADDRESS);
  Wire.write(0x03);
  Wire.endTransmission();

  //Send data request of 6 bytes
  Wire.requestFrom(HMC5883_ADDRESS,6);

  if(Wire.available() >= 6){
    //Read X
    Compass::rawMag[0]=(Wire.read()<<8 | Wire.read()) - Compass::offset[0];         //Read upper 8 bits (MSB)
    //Read Z
    Compass::rawMag[2]=(Wire.read()<<8 | Wire.read()) - Compass::offset[1];
    //Read Y
    Compass::rawMag[1]=(Wire.read()<<8 | Wire.read()) - Compass::offset[2];
  }
}

void Compass::initialize(sensorGain_t gain,short offsetX,short offsetY,short offsetZ,float declinationDegree){
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
  Compass::offset[0]=offsetX;
  Compass::offset[1]=offsetY;
  Compass::offset[2]=offsetZ;
  Compass::heading=0;
  Compass::declinationAngle=declinationDegree/180*PI;

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

void Compass::debug(void){
  //Used to generate datapoints for calibration
  short datapoint[6][3];

  if(Serial){
      Serial.println("****************************************************");
      Serial.println("Magnetometer calibration mode active...");
      Serial.println("****************************************************");
      Serial.println("Program will take six data points.");
      Serial.println("Make sure magnetometer is stable and upright!");
      Serial.println("Press any key to continue...");
      //First 4 datapoints at 90 degrees apart
      for(int i=0;i<4;i++){
        while (!Serial.available());
        Serial.parseInt();
        Compass::read();
        datapoint[i][0]=Compass::rawMag[0];
        datapoint[i][1]=Compass::rawMag[1];
        datapoint[i][2]=Compass::rawMag[2];
        Serial.print("Datapoint ");
        Serial.print(i+1);
        Serial.println(" set.");
        if(i<3) Serial.println("Rotate sensor 90° and press any key...");
      }
      //Last 2 datapoints for additional information
      Serial.println("Rotate sensor some and press any key...");
      for(int i=4;i<6;i++){
        while (!Serial.available());
        Serial.parseInt();
        Compass::read();
        datapoint[i][0]=Compass::rawMag[0];
        datapoint[i][1]=Compass::rawMag[1];
        datapoint[i][2]=Compass::rawMag[2];
        Serial.print("Datapoint ");
        Serial.print(i+1);
        Serial.println(" set.");
        if(i<5) Serial.println("Rotate sensor some and press any key...");
      }

      Serial.println("****************************************************");
      Serial.println("Reading out datapoints...");
      Serial.println("DATAPOINT     X     Y     Z     ");
      for(int i=0;i<6;i++){
        Serial.print(i+1);
        Serial.print("       ");
        Serial.print(datapoint[i][1]);
        Serial.print("   ");
        Serial.print(datapoint[i][2]);
        Serial.print("   ");
        Serial.println(datapoint[i][3]);
      }
      Serial.println();
      Serial.println("Data ready for matlab tool.");
      Serial.println("Exiting calibration mode...");
      Serial.println("****************************************************");
  }
}

void Compass::sendSerial(void){
    //Prints the time and angle data to serial
    Serial.print("Fixed Heading ");
    Serial.print(Compass::fixHeading);
    Serial.print(" Tilt Compensated Heading ");
    Serial.print(Compass::heading);
    Serial.print(" Bias ");
    Serial.print(Compass::heading-Compass::fixHeading);
    //Serial.println();
}

#endif
