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
      void begin(ROTORCONTROL rotors);
      void update();
    private:
      POSITIONCONTROL *p_con_address;
      HEIGHTCONTROL *h_con_address;
      ROTORCONTROL *r_con_address;
  };

  //BLE UUIDS have to be defined externally to work properly
  BLEPeripheral quad101_peripheral;
  BLEService ble_telemetrics_service("2E8C6277-2DDE-4D80-8C4B-629876703C70");
  //Positional Controller
  BLEFloatCharacteristic ble_pcon_KP("2E8C6277-2DDE-4D80-8C4B-629876703C71", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_pcon_TI("2E8C6277-2DDE-4D80-8C4B-629876703C72", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_pcon_TD("2E8C6277-2DDE-4D80-8C4B-629876703C73", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_pcon_TDD("2E8C6277-2DDE-4D80-8C4B-629876703C74", BLERead | BLEWrite);
  //Height Controller
  BLEFloatCharacteristic ble_hcon_KP("2E8C6277-2DDE-4D80-8C4B-629876703C75", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_hcon_TI("2E8C6277-2DDE-4D80-8C4B-629876703C76", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_hcon_TD("2E8C6277-2DDE-4D80-8C4B-629876703C77", BLERead | BLEWrite);
  BLEFloatCharacteristic ble_hcon_HEIGHT("2E8C6277-2DDE-4D80-8C4B-629876703C78", BLERead | BLEWrite);
  //
  BLEUnsignedCharCharacteristic ble_startstop("2E8C6277-2DDE-4D80-8C4B-629876703C79", BLEWrite);


/*==================================================================*/
  //Functions
  void BLE::begin(ROTORCONTROL rotors){
    if(Serial) Serial.print("Starting BLE service...");

    // Set the data pointer
    r_con_address=&rotors;
    p_con_address=&rotors.positionController;
    h_con_address=&rotors.heightController;

    // Initialize ble enviroment
    quad101_peripheral.setLocalName("quad101");
    quad101_peripheral.setAdvertisedServiceUuid(ble_telemetrics_service.uuid());
    quad101_peripheral.addAttribute(ble_telemetrics_service);
    //
    quad101_peripheral.addAttribute(ble_pcon_KP);
    quad101_peripheral.addAttribute(ble_pcon_TI);
    quad101_peripheral.addAttribute(ble_pcon_TD);
    quad101_peripheral.addAttribute(ble_pcon_TDD);
    //
    quad101_peripheral.addAttribute(ble_hcon_KP);
    quad101_peripheral.addAttribute(ble_hcon_TI);
    quad101_peripheral.addAttribute(ble_hcon_TD);
    quad101_peripheral.addAttribute(ble_hcon_HEIGHT);
    //
    quad101_peripheral.addAttribute(ble_startstop);

    //Set data for the first time
    ble_pcon_KP.setValue(p_con_address->K_P);
    ble_pcon_TI.setValue(p_con_address->T_I);
    ble_pcon_TD.setValue(p_con_address->T_D);
    ble_pcon_TDD.setValue(p_con_address->T_DD);

    ble_hcon_KP.setValue(h_con_address->K_P_HEIGHT);
    ble_hcon_TI.setValue(h_con_address->T_I_HEIGHT);
    ble_hcon_TD.setValue(h_con_address->T_D_HEIGHT);
    ble_hcon_HEIGHT.setValue(h_con_address->targetHeight);

    if(BLE_AUTOSTART){
      ble_startstop.setValue(0);
      rotors.safetyModeOn=0; //0 means start
    }else{
      ble_startstop.setValue(1);
      rotors.safetyModeOn=1; //1 means stop
    }

    //Start advertising
    quad101_peripheral.begin();
    if(Serial) Serial.println(" DONE");
  }

  void BLE::update(){
    //Data download
    //p
    if(ble_pcon_KP.written()){
      p_con_address->K_P=ble_pcon_KP.value();
    }

    if(ble_pcon_TI.written()){
      p_con_address->T_I=ble_pcon_TI.value();
    }

    if(ble_pcon_TD.written()){
      p_con_address->T_D=ble_pcon_TD.value();
    }

    if(ble_pcon_TDD.written()){
      p_con_address->T_DD=ble_pcon_TDD.value();
    }
    //h
    if(ble_hcon_KP.written()){
      h_con_address->K_P_HEIGHT=ble_hcon_KP.value();
    }

    if(ble_hcon_TI.written()){
      h_con_address->T_I_HEIGHT=ble_hcon_TI.value();
    }

    if(ble_hcon_TD.written()){
      h_con_address->T_D_HEIGHT=ble_hcon_TD.value();
    }

    if(ble_hcon_HEIGHT.written()){
      h_con_address->targetHeight=ble_hcon_HEIGHT.value();
    }
    //startstop
    if(ble_startstop.written()){
      r_con_address->safetyModeOn=ble_startstop.value();
      if(Serial) if(r_con_address->safetyModeOn) Serial.println("!!! Emergency stop initiated !!!");
    }

  }

#endif
