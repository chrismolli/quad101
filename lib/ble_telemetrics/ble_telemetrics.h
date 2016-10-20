#ifndef BLE_TELEMETRICS_H
#define BLE_TELEMETRICS_H
/*
    Lib for using the integrated BLE module on the Arduino 101 to send
    telemtrics and read commands for PID tuning.
*/


/*==================================================================*/
  //Extern libraries
  #include "../params.h"
  #include "../sensors/sensors.h"
  #include "../control/RotorControl.h"
  #include <CurieBLE.h>

/*==================================================================*/
  //Class definition
  class BLE{
    public:
      void begin(ROTORCONTROL rotors, SENSORS sensors);
      void update();
    private:
      POSITIONCONTROL *p_con_address;
      HEIGHTCONTROL *h_con_address;
      SENSORS *sensors_address;
  };

  //BLE UUIDS have to be defined externally to work properly
  BLEPeripheral quad101_peripheral;
  BLEService ble_telemetrics_service("2E8C6277-2DDE-4D80-8C4B-629876703C70");
  //Telemetrics
  BLEFloatCharacteristic ble_roll("2E8C6277-2DDE-4D80-8C4B-629876703C71", BLERead | BLENotify);
  BLEFloatCharacteristic ble_pitch("2E8C6277-2DDE-4D80-8C4B-629876703C72", BLERead | BLENotify);
  BLEFloatCharacteristic ble_jaw("2E8C6277-2DDE-4D80-8C4B-629876703C73", BLERead | BLENotify);
  BLEFloatCharacteristic ble_height("2E8C6277-2DDE-4D80-8C4B-629876703C74", BLERead | BLENotify);
  //Positional Controller
  BLEFloatCharacteristic ble_pcon_KP("2E8C6277-2DDE-4D80-8C4B-629876703C75", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_pcon_TI("2E8C6277-2DDE-4D80-8C4B-629876703C76", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_pcon_TD("2E8C6277-2DDE-4D80-8C4B-629876703C77", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_pcon_TDD("2E8C6277-2DDE-4D80-8C4B-629876703C78", BLERead | BLEWrite);
  //Height Controller
  BLEFloatCharacteristic ble_hcon_KP("2E8C6277-2DDE-4D80-8C4B-629876703C79", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_hcon_TI("2E8C6277-2DDE-4D80-8C4B-629876703C80", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_hcon_TD("2E8C6277-2DDE-4D80-8C4B-629876703C81", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_hcon_HEIGHT("2E8C6277-2DDE-4D80-8C4B-629876703C82", BLERead | BLEWrite);


/*==================================================================*/
  //Functions
  void BLE::begin(ROTORCONTROL rotors, SENSORS sensors){
    // Set the data pointer
    p_con_address=&rotors.positionController;
    h_con_address=&rotors.heightController;
    sensors_address=&sensors;

    // Initialize ble enviroment
    quad101_peripheral.setLocalName("quad101");
    quad101_peripheral.setAdvertisedServiceUuid(ble_telemetrics_service.uuid());
    quad101_peripheral.addAttribute(ble_telemetrics_service);
    //
    quad101_peripheral.addAttribute(ble_roll);
    quad101_peripheral.addAttribute(ble_pitch);
    quad101_peripheral.addAttribute(ble_jaw);
    quad101_peripheral.addAttribute(ble_height);
    //
    quad101_peripheral.addAttribute(ble_pcon_KP);
    quad101_peripheral.addAttribute(ble_pcon_TI);
    quad101_peripheral.addAttribute(ble_pcon_TD);
    quad101_peripheral.addAttribute(ble_pcon_TDD);
    //
    /*quad101_peripheral.addAttribute(ble_hcon_KP);
    quad101_peripheral.addAttribute(ble_hcon_TI);
    quad101_peripheral.addAttribute(ble_hcon_TD);
    quad101_peripheral.addAttribute(ble_hcon_HEIGHT);*/

    //Set data for the first time
    ble_roll.setValue(sensors_address->imu.rot[0]);
    ble_pitch.setValue(sensors_address->imu.rot[1]);
    ble_jaw.setValue(sensors_address->imu.rot[2]);
    ble_height.setValue(sensors_address->usr.height);

    ble_pcon_KP.setValue(p_con_address->K_P);
    ble_pcon_TI.setValue(p_con_address->T_I);
    ble_pcon_TD.setValue(p_con_address->T_D);
    ble_pcon_TDD.setValue(p_con_address->T_DD);

    /*ble_hcon_KP.setValue(h_con_address->K_P_HEIGHT);
    ble_hcon_TI.setValue(h_con_address->T_I_HEIGHT);
    ble_hcon_TD.setValue(h_con_address->T_D_HEIGHT);
    ble_hcon_HEIGHT.setValue(h_con_address->targetHeight);*/

    //Start advertising
    if(Serial) Serial.print("Starting BLE service...");
    quad101_peripheral.begin();
    if(Serial) Serial.println(" DONE");
  }

  void BLE::update(){
    //Telemetrics upload
    ble_roll.setValue(sensors_address->imu.rot[0]);
    ble_pitch.setValue(sensors_address->imu.rot[0]);
    ble_jaw.setValue(sensors_address->imu.rot[0]);
    ble_height.setValue(sensors_address->imu.rot[0]);

    //Data download
    //if(ble_pcon_KP.written()) pcon_KP=ble_pcon_KP.value();
    // TODO Add data download
  }

#endif
