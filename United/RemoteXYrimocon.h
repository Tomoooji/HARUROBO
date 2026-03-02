#pragma once

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
//#define REMOTEXY_MODE__ESP32CORE_BLUETOOTH
//#include <BluetoothSerial.h>
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>


// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 126 bytes V19 
  { 255,10,0,0,0,119,0,19,0,0,0,0,31,2,106,200,200,84,1,1,
  8,0,5,209,12,143,143,21,19,60,60,32,2,26,31,5,5,43,143,143,
  112,7,60,60,32,2,26,31,1,235,5,57,57,8,3,24,24,0,2,31,
  0,1,15,7,57,57,70,3,24,24,0,2,31,0,1,71,52,57,57,111,
  67,24,15,1,2,31,0,1,71,122,57,57,150,67,26,15,1,2,31,0,
  1,69,17,57,57,172,7,24,24,0,2,31,0,1,69,91,57,57,172,38,
  24,24,0,2,31,0 };
  
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
