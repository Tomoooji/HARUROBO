#define BLE//or BTCL
#include "RemoteXYinputs.h"


void setup(){
  RemoteXY_Init ();  // initialization by macros 

}

void loop(){
  RemoteXYEngine.handler ();   

  // do not call delay(), use instead RemoteXYEngine.delay() 
}
