#pragma once

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#if defined(REOMOTEXY_BTCL)
  #define REMOTEXY_MODE__ESP32CORE_BLUETOOTH
  #include <BluetoothSerial.h>
#elif defined(REMOTEXY_BLE)
  #define REMOTEXY_MODE__ESP32CORE_BLE
  #include <BLEDevice.h>
#endif

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "HonBanYou"

#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 338 bytes V19 
  { 255,17,0,0,0,75,1,19,0,0,0,0,31,2,106,200,200,84,1,1,
  19,0,5,209,12,143,143,13,20,60,60,32,2,26,31,5,5,43,143,143,
  126,21,60,60,32,2,26,31,1,235,5,57,57,6,5,20,20,0,2,31,
  229,183,166,0,1,15,7,57,57,57,6,20,20,0,2,31,229,143,179,0,
  1,71,52,57,57,73,66,22,13,1,2,31,233,150,137,0,1,71,122,57,
  57,104,65,22,13,1,2,31,233,150,139,0,1,69,17,57,57,185,27,13,
  20,1,2,31,226,134,145,0,1,69,91,57,57,185,52,13,20,1,2,31,
  226,134,147,0,1,34,0,57,57,128,8,19,14,1,2,31,233,150,137,0,
  1,58,249,57,57,166,8,19,14,1,2,31,233,150,139,0,3,30,26,39,
  105,101,2,17,61,4,2,26,129,242,7,71,29,28,10,27,9,64,17,232,
  182,179,229,155,158,227,130,138,0,129,28,50,71,29,88,75,24,8,64,17,
  227,130,132,227,129,144,227,130,137,0,129,43,0,71,29,144,12,27,9,64,
  17,227,131,143,227,131,179,227,131,137,0,10,47,43,57,57,189,0,11,11,
  49,4,25,31,79,78,0,31,79,70,70,0,1,235,79,57,57,5,65,17,
  17,0,2,31,233,129,133,0,1,99,69,7,48,82,12,13,20,1,2,31,
  226,134,145,0,1,99,128,7,48,82,39,13,20,1,2,31,226,134,147,0,
  129,52,10,63,21,79,32,18,9,64,17,227,129,164,227,130,129,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  int8_t joystick_02_x; // from -100 to 100
  int8_t joystick_02_y; // from -100 to 100
  uint8_t button_01; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_02; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_03; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_04; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_05; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_06; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_07; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_08; // =1 if button pressed, else =0, from 0 to 1
  uint8_t selectorSwitch_01; // from 0 to 4
  uint8_t pushSwitch_02; // =1 if state is ON, else =0, from 0 to 1
  uint8_t button_09; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_10; // =1 if button pressed, else =0, from 0 to 1
  uint8_t button_11; // =1 if button pressed, else =0, from 0 to 1

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   

#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

//  RemoteXY_Init ();  // initialization by macros 
//  RemoteXYEngine.handler ();   
//  RemoteXYEngine.delay() 
