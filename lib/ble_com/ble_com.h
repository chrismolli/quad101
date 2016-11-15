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
  class BLE_COM{
    public:
      void begin(ROTORCONTROL *rotors);
      void update();
    private:
      //uint8_t _connection_flag;
      //BLECentralHelper _central;

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
  BLEUnsignedCharCharacteristic ble_startstop("2E8C6277-2DDE-4D80-8C4B-629876703C79", BLERead | BLEWrite);


/*==================================================================*/
  //Functions
  void BLE_COM::begin(ROTORCONTROL *rotors){
    if(Serial) Serial.print("Starting BLE service...");

    // Set the data pointer
    r_con_address=rotors;
    p_con_address=&rotors->positionController;
    h_con_address=&rotors->heightController;

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
    ble_pcon_KP.setValue(p_con_address->K_P_ROLL);
    ble_pcon_TI.setValue(p_con_address->T_I_ROLL);
    ble_pcon_TD.setValue(p_con_address->T_D_ROLL);
    ble_pcon_TDD.setValue(p_con_address->T_DD_ROLL);

    ble_hcon_KP.setValue(h_con_address->K_P_HEIGHT);
    ble_hcon_TI.setValue(h_con_address->T_I_HEIGHT);
    ble_hcon_TD.setValue(h_con_address->T_D_HEIGHT);
    ble_hcon_HEIGHT.setValue(h_con_address->targetHeight);

    ble_startstop.setValue(0);

    //Start advertising
    //_connection_flag=0;
    //_central=NULL;
    quad101_peripheral.begin();
    if(Serial) Serial.println(" DONE");
  }

  void BLE_COM::update(){
    /*
    //Listen for centrals to connect
    _central = quad101_peripheral.central();
    if(_central){
      if(_connection_flag==0 && Serial){
        Serial.print("Connected to central: ");
        Serial.println(_central.address());
      }
      _connection_flag=1;
    }

    //Auto stop when central disconnects
    if(!_central && _connection_flag){
      _connection_flag=0;
      r_con_address->safetyModeOn=1;
      ble_startstop.setValue(1);
      if(Serial) Serial.println("!!! Emergency stop initiated (Connection lost) !!!");
    }
    */

    //Data download
    //p-controller
    if(ble_pcon_KP.written()) p_con_address->K_P_ROLL=ble_pcon_KP.value();
    if(ble_pcon_TI.written()) p_con_address->T_I_ROLL=ble_pcon_TI.value();
    if(ble_pcon_TD.written()) p_con_address->T_D_ROLL=ble_pcon_TD.value();
    if(ble_pcon_TDD.written()) p_con_address->T_DD_ROLL=ble_pcon_TDD.value();

    //h-controller
    if(ble_hcon_KP.written()) h_con_address->K_P_HEIGHT=ble_hcon_KP.value();
    if(ble_hcon_TI.written()) h_con_address->T_I_HEIGHT=ble_hcon_TI.value();
    if(ble_hcon_TD.written()) h_con_address->T_D_HEIGHT=ble_hcon_TD.value();
    if(ble_hcon_HEIGHT.written()) h_con_address->targetHeight=ble_hcon_HEIGHT.value();

    //Manual startstop function
    if(ble_startstop.written()){
      r_con_address->safetyModeOn=ble_startstop.value();
      if(Serial && r_con_address->safetyModeOn) Serial.println("!!! Emergency stop initiated !!!");
    }

  }

#endif
