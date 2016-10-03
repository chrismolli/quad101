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

    private:
      void beginBLE(void);
      void updateBLE(void);
  };

  //BLE Telemetrics
  BLEPeripheral quad101_peripheral;
  BLEService ble_telemetrics_service("2E8C6277-2DDE-4D80-8C4B-629876703C70");
  BLEFloatCharacteristic ble_roll("2E8C6277-2DDE-4D80-8C4B-629876703C7R", BLERead | BLENotify);
  BLEFloatCharacteristic ble_pitch("2E8C6277-2DDE-4D80-8C4B-629876703C7P", BLERead | BLENotify);
  BLEFloatCharacteristic ble_jaw("2E8C6277-2DDE-4D80-8C4B-629876703C7J", BLERead | BLENotify);
  BLEFloatCharacteristic ble_height("2E8C6277-2DDE-4D80-8C4B-629876703C7H", BLERead | BLENotify);

/*==================================================================*/
  //Functions
  void SENSORS::begin(void){
    imu.begin();
    if(BMP_PLUGGED_IN) bmp.begin();
    if(USR_PLUGGED_IN) usr.begin();
    if(BLE_TELEMETRICS_ON) beginBLE();
  }

  void SENSORS::update(void){
    imu.update(SAMPLE_RATE);
    if(BMP_PLUGGED_IN) bmp.update();
  }

  void SENSORS::updateSlow(void){
    if(USR_PLUGGED_IN) usr.pulseOut();
    if(BLE_TELEMETRICS_ON) updateBLE();
  }

  void SENSORS::beginBLE(void){
    quad101_peripheral.setLocalName("quad101");
    quad101_peripheral.setAdvertisedServiceUuid(ble_telemetrics_service.uuid());
    quad101_peripheral.addAttribute(ble_telemetrics_service);
    quad101_peripheral.addAttribute(ble_roll);
    quad101_peripheral.addAttribute(ble_pitch);
    quad101_peripheral.addAttribute(ble_jaw);
    quad101_peripheral.addAttribute(ble_height);

    ble_roll.setValue(imu.rot[0]);
    ble_pitch.setValue(imu.rot[1]);
    ble_jaw.setValue(imu.rot[2]);
    ble_height.setValue(usr.height);

    if(Serial) Serial.println("Starting BLE service");
    quad101_peripheral.begin();
  }

  void SENSORS::updateBLE(){
    ble_roll.setValue(imu.rot[0]);
    ble_pitch.setValue(imu.rot[1]);
    ble_jaw.setValue(imu.rot[2]);
    ble_height.setValue(usr.height);
  }


#endif
