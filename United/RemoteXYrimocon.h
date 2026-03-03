#pragma once

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library
#if defined(RemoteXY_BTCL)
  #define REMOTEXY_MODE__ESP32CORE_BLUETOOTH
  #include <BluetoothSerial.h>
#elif defined(RemoteXY_BLE)
  #define REMOTEXY_MODE__ESP32CORE_BLE
  #include <BLEDevice.h>
#endif

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "HaruRobo"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 152 bytes V19 
  { 255,12,0,0,0,145,0,19,0,0,0,0,31,2,106,200,200,84,1,1,
  10,0,5,209,12,143,143,14,15,60,60,32,2,26,31,5,5,43,143,143,
  113,16,60,60,32,2,26,31,1,235,5,57,57,1,3,24,24,0,2,31,
  0,1,15,7,57,57,62,4,24,24,0,2,31,0,1,71,52,57,57,66,
  66,24,15,1,2,31,0,1,71,122,57,57,100,66,26,15,1,2,31,0,
  1,69,17,57,57,173,19,24,24,0,2,31,0,1,69,91,57,57,173,50,
  24,24,0,2,31,0,1,34,0,57,57,106,0,24,24,0,2,31,0,1,
  58,249,57,57,155,1,24,24,0,2,31,0 };
  
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
