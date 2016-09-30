#ifndef BMP180_h
#define BMP180_h
/*
  Custom library for using the BMP180 pressure sensor.
  Smart update function skips long time delays due to high oversampling values.

  SAMPLING          DELAY [ms]
  ---------------------------------------------------------------------
  ULTRALOW          5
  STANDART          8
  HIGH              14
  ULTRAHIGH         26
  TEMPMEASURE       5

  BMP180::update(); is an all in one update function for temperature,pressure
  and altitude.
*/

/*==================================================================*/
  //Extern librarys
  #include "Wire.h"
  #include "../params.h"
  #include <Arduino.h>
/*==================================================================*/
  // i2c address
  #define BMP180_I2C_ADDRESS							0x77

  #ifndef SEA_LEVEL_PRESSURE
    #define SEA_LEVEL_PRESSURE 101325
  #endif

  //Data registers
  typedef enum{
    BMP180_PRESSURE_DATA    			       		= 0xF6,
    BMP180_TEMP_DATA    			          		= 0xF6,
    BMP180_CONTROL_REGISTER					      	= 0xF4,
    BMP180_SOFT_RESET_REGISTER					    = 0xE0,
    BMP180_CHIP_ID_REGISTER						      = 0xD0,
    BMP180_CALIBRATION_DATA_AC1				      =	0xAA,
    BMP180_CALIBRATION_DATA_AC2				      =	0xAC,
    BMP180_CALIBRATION_DATA_AC3				      =	0xAE,
    BMP180_CALIBRATION_DATA_AC4				      =	0xB0,
    BMP180_CALIBRATION_DATA_AC5				      =	0xB2,
    BMP180_CALIBRATION_DATA_AC6				      =	0xB4,
    BMP180_CALIBRATION_DATA_B1		   		    =	0xB6,
    BMP180_CALIBRATION_DATA_B2			  	    =	0xB8,
    BMP180_CALIBRATION_DATA_MB			  	    =	0xBA,
    BMP180_CALIBRATION_DATA_MC			  	    =	0xBC,
    BMP180_CALIBRATION_DATA_MD		   		    =	0xBE
  }BMP_REGISTER_t;

/*==================================================================*/
  //Sampling bytes
  typedef enum
  {
    SAMPL_ULTRA_LOW_POWER			    = 0x00,
    SAMPL_STANDARD				        = 0x01,
    SAMPL_HIGH_RESOLUTION			    = 0x02,
    SAMPL_ULTRA_HIGH_RESOLUTION	  = 0x03
  }BMP_SAMPLING_RES_t;

  //Measure Modes Flag
  typedef enum
  {
    BMP_CURRENT_MEASURING_NONE			    = 0x00,
    BMP_CURRENT_MEASURING_TEMP				  = 0x01,
    BMP_CURRENT_MEASURING_PRES			    = 0x02,
    BMP_TEMPERATURE_DELAY               = 0x04
  }MEASURE_FLAGS_t;

/*==================================================================*/
  //Class definition
  class BMP180{
    public:
      long pressure;
      long temperature;
      int  altitude;

      void begin();
      void setSamplingRes(BMP_SAMPLING_RES_t res);
      void update();
      void sendSerial();

    private:
      uint16_t oss; //Sampling Rate
      long  raw_pressure;
      long  raw_temperature;

      uint8_t  measure_flag; //0 None 1 Temp 2 Pressure
      unsigned long measure_time;
      uint8_t  pressure_delay;

      //Calibration data stack
      int16_t _AC1;
      int16_t _AC2;
      int16_t _AC3;
      int16_t _B1;
      int16_t _B2;
      int16_t _MB;
      int16_t _MC;
      int16_t _MD;
      uint16_t _AC4;
      uint16_t _AC5;
      uint16_t _AC6;

      int16_t  _B5;

      //I2C
      void write(uint8_t reg, unsigned char hex);
      void read8(uint8_t reg, uint8_t *val);
      void read16(uint8_t reg, uint16_t *val);
      void readS16(uint8_t reg, int16_t *val);
  };

/*==================================================================*/
  //Functions

  //Basic I2C functions
  void BMP180::write(uint8_t reg, unsigned char hex){
    /*
     *  Writes I2C message to bmp180
     */
      Wire.beginTransmission(BMP180_I2C_ADDRESS);
      Wire.write(reg);
      Wire.write(hex);
      Wire.endTransmission();
  }

  void BMP180::read8(uint8_t reg, uint8_t *val) {
    //Setting the pointer to the wanted data register
    Wire.beginTransmission(BMP180_I2C_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();
    //Request 1 byte (8 bit)
    Wire.requestFrom(BMP180_I2C_ADDRESS, 1);
    *val = Wire.read();
    Wire.endTransmission();
  }

  void BMP180::read16(uint8_t reg, uint16_t *val) {
    //Setting the pointer to the wanted data register
    Wire.beginTransmission(BMP180_I2C_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();
    //Request 2 bytes (16 bit)
    Wire.requestFrom(BMP180_I2C_ADDRESS, 2);
    *val = (Wire.read() << 8) | Wire.read();
    Wire.endTransmission();
  }

  void BMP180::readS16(uint8_t reg, int16_t *val) {
    uint16_t i;
    BMP180::read16(reg, &i);
    *val = (int16_t)i;
  }

  //Sensor specific functions
  void BMP180::setSamplingRes(BMP_SAMPLING_RES_t res){
    oss=res;
    switch (res) {
      case SAMPL_ULTRA_LOW_POWER:
        pressure_delay=4;
        break;
      case SAMPL_STANDARD:
        pressure_delay=7;
        break;
      case SAMPL_HIGH_RESOLUTION:
        pressure_delay=13;
        break;
      case SAMPL_ULTRA_HIGH_RESOLUTION:
        pressure_delay=25;
        break;
      default:
        pressure_delay=25;
        break;
    }
  }

  void BMP180::begin(){
      Wire.begin();

      //Reading calibration data
      readS16(BMP180_CALIBRATION_DATA_AC1,&_AC1);
      readS16(BMP180_CALIBRATION_DATA_AC2,&_AC2);
      readS16(BMP180_CALIBRATION_DATA_AC3,&_AC3);
      read16(BMP180_CALIBRATION_DATA_AC4,&_AC4);
      read16(BMP180_CALIBRATION_DATA_AC5,&_AC5);
      read16(BMP180_CALIBRATION_DATA_AC6,&_AC6);

      readS16(BMP180_CALIBRATION_DATA_B1,&_B1);
      readS16(BMP180_CALIBRATION_DATA_B2,&_B2);

      readS16(BMP180_CALIBRATION_DATA_MB,&_MB);
      readS16(BMP180_CALIBRATION_DATA_MC,&_MC);
      readS16(BMP180_CALIBRATION_DATA_MD,&_MD);

      //Setting sampling res
      setSamplingRes(SAMPL_ULTRA_HIGH_RESOLUTION);
      measure_flag=0;
  }

  void BMP180::update(){
    switch (measure_flag) {
      case BMP_CURRENT_MEASURING_NONE:
        //Send temperature measure command
        write(BMP180_CONTROL_REGISTER, 0x2E);
        measure_time = millis();
        measure_flag = BMP_CURRENT_MEASURING_TEMP;
        break;

      case BMP_CURRENT_MEASURING_TEMP:
        if ((millis()-measure_time) > BMP_TEMPERATURE_DELAY) {
          //Reading temperature measurement
          uint8_t msb,lsb;
          read8(BMP180_TEMP_DATA,&msb);
          read8(BMP180_TEMP_DATA+1,&lsb);
          raw_temperature = (msb<<8) + lsb;

          //Compensate temperature algorithm
          long X1, X2;
          X1 = (raw_temperature - _AC6) * _AC5 >> 15;
        	X2 = (_MC << 11) / (X1 + _MD);
          _B5 = X1 + X2;
          temperature = (_B5 + 8) >> 4;

          //Send pressure measure command
          write(BMP180_CONTROL_REGISTER, (0x34+(oss<<6)));
          measure_time = millis();
          measure_flag = BMP_CURRENT_MEASURING_PRES;
        }break;

      case BMP_CURRENT_MEASURING_PRES:
        if ((millis()-measure_time) > pressure_delay) {
          //Read pressure measurement
          uint8_t msb,lsb,xlsb;
          read8(BMP180_PRESSURE_DATA,&msb);
          read8(BMP180_PRESSURE_DATA+1,&lsb);
          read8(BMP180_PRESSURE_DATA+2,&xlsb);
          raw_pressure = ((msb<<16) + (lsb<<8) + xlsb) >> (8-oss);

          //Compensate pressure algorithm
          long X1, X2, B6, X3, B3;
        	unsigned long B4, B7;
        	B6 = _B5 - 4000;
        	X1 = (_B2 * (B6 * B6 >> 12)) >> 11;
        	X2 = (_AC2 * B6) >> 11;
        	X3 = X1 + X2;
        	B3 = (((_AC1 * 4 + X3) << oss) + 2) >> 2;
        	X1 = _AC3 * B6 >> 13;
        	X2 = (_B1 * (B6 * B6 >> 12)) >> 16;
        	X3 = ((X1 + X2) + 2) >> 2;
        	B4 = _AC4 * (unsigned long)(X3 + 32768) >> 15;
        	B7 = ((unsigned long)raw_pressure - B3) * (50000 >> oss);
        	if (B7 < 0x80000000)
        		pressure = (B7 * 2) / B4;
        	else
        		pressure = (B7 / B4) * 2;
        	X1 = (pressure >> 8) * (pressure >> 8);
        	X1 = (X1 * 3038) >> 16;
        	X2 = (-7357 * pressure) >> 16;
        	pressure = pressure + ((X1 + X2 + 3791) >> 4);

          //Calculate altitude
          altitude = 44330 * (1 - pow((double)pressure/SEA_LEVEL_PRESSURE,1/5.255));

          //Send temperature measure command
          write(BMP180_CONTROL_REGISTER, 0x2E);
          measure_time = millis();
          measure_flag = BMP_CURRENT_MEASURING_TEMP;

          //Debug
          //sendSerial();
        }break;

      default:
        measure_flag=BMP_CURRENT_MEASURING_NONE;
        break;
    }
  }

  void BMP180::sendSerial(){
    Serial.print("RAWTEMP: ");
    Serial.print(raw_temperature);
    Serial.print(" RAWPRESS: ");
    Serial.print(raw_pressure);
    Serial.print(" TEMP: ");
    Serial.print(temperature);
    Serial.print(" PRESS: ");
    Serial.print(pressure);
    Serial.print(" ALTITUDE: ");
    Serial.print(altitude);
    Serial.println();
  }

#endif
