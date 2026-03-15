#pragma once
#include <Arduino.h>

#if defined(PS4_CONTROLLER)
  #include <PS4Controller.h>
  constexpr char MAC_PS4CON[] = "40:99:22:27:b5:e8";
  //constexpr float range_othogonal = radians(40); // 前後左右に±50°,斜めは±40°
  constexpr int range_ignoreLstick = 50;
  constexpr int line_RL2pushed = 75;
  constexpr int range_ignoreRstick = 20;

  inline void PS4Input(){
    disconnect_button = PS4.Touchpad()? !disconnect_button: disconnect_button;//
    connection_flag = PS4.isConnected() && !disconnect_button;
    appeal_button = PS4.Share() && PS4.Options();
    leg_joystick_x = PS4.LStickX();
    leg_joystick_y = PS4.LStickY();
    leg_button_R = PS4.R2Value() > line_RL2pushed;
    leg_button_L = PS4.L2Value() > line_RL2pushed;
    leg_button_shift = PS4.Share() && !PS4.Options();/////////////
    yagura_L = PS4.Left();
    yagura_R = PS4.Right();
    arm_joystick_x = -PS4.RStickX();
    arm_joystick_y = PS4.RStickY();
    arm_button_UP =PS4.Circle();
    arm_button_DOWN =PS4.Cross();
    arm_button_init = PS4.Square() && PS4.Triangle();
    arm_button_pick = PS4.Square() && !PS4.Triangle();
    arm_button_drop = !PS4.Square() &&PS4.Triangle();
    finger_button_UP = PS4.R1();////
    finger_button_DOWN=PS4.L1();////
    clow_button_UP = PS4.Up();
    clow_button_DOWN = PS4.Down();
  }

#elif defined(REMOTEXY_BTCL) || defined(REMOTEXY_BLE)
  #include "Rimocon_RemoteXY.h"
  //constexpr float range_othogonal = radians(25); // 前後左右に±50°,斜めは±40°
  constexpr int range_ignoreLstick = 20;
  constexpr int range_ignoreRstick = 20;

  // update controller values
  inline void RemoteXYInput(){
    connection_flag = RemoteXY.connect_flag && RemoteXY.pushSwitch_02;
    appeal_button = RemoteXY.button_03 && RemoteXY.button_04;
    leg_joystick_x = RemoteXY.joystick_01_x;
    leg_joystick_y = RemoteXY.joystick_01_y;
    leg_button_R = RemoteXY.button_01;
    leg_button_L = RemoteXY.button_02;
    leg_button_shift = RemoteXY.button_09;////////
    yagura_L = RemoteXY.button_03;
    yagura_R = RemoteXY.button_04;
    arm_joystick_x = RemoteXY.joystick_02_x;
    arm_joystick_y = RemoteXY.joystick_02_y;
    arm_button_UP = RemoteXY.button_05;
    arm_button_DOWN=RemoteXY.button_06;
    arm_button_init = RemoteXY.selectorSwitch_01 == 1;
    arm_button_pick = RemoteXY.selectorSwitch_01 == 2;
    arm_button_drop = RemoteXY.selectorSwitch_01 == 3;
    finger_button_UP = RemoteXY.button_07;
    finger_button_DOWN=RemoteXY.button_08;
    clow_button_UP = RemoteXY.button_10;
    clow_button_DOWN = RemoteXY.button_11;
  }

#elif defined(SERIAL_CONTROLLER)
  #include "Rimocon_Serial.h"
  constexpr float range_othogonal = 0.0;
  constexpr int range_ignoreRstick = 0;
  constexpr int range_ignoreLstick = 0;

#endif

inline int sign(auto x){return (x>0)-(x<0);}

inline bool isStickmoved(int xval, int yval, int range){
  return sq(xval)+sq(yval) > sq(range);
}