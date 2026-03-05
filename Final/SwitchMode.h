#pragma once
#include <Arduino.h>

#if defined(PS4_CONTROLLER)
  #include <PS4Controller.h>

  constexpr char MAC_PS4CON[] = "e4:65:b8:d8:d4:80";

  inline void PS4Input(){
    connection_flag = PS4.isConnected();
    leg_joystick_x = PS4.LStickX();
    leg_joystick_y = PS4.LStickY();
    leg_button_R = PS4.R2Value() > line_RL2pushed;
    leg_button_L = PS4.L2Value() > line_RL2pushed;
    leg_button_shift = PS4.L3();
    yagura_L = PS4.Left();
    yagura_R = PS4.Right();
    arm_joystick_x = PS4.RStickX();
    arm_joystick_y = PS4.RStickY();
    arm_button_UP =PS4.Circle();
    arm_button_DOWN =PS4.Cross();
    hand_button_UP = PS4.L1();
    hand_button_DOWN=PS4.R1();
    //arm_button_init = PS4.Up();
    //arm_button_pick = PS4.Square();
    //arm_button_drop = PS4.Triangle();
  }

#elif defined(REMOTEXY_BTCL) || defined(REMOTEXY_BLE)
  #include "RemoteXYrimocon.h"
  // update controller values
  inline void RemoteXYInput(){
    connection_flag = RemoteXY.connect_flag;
    leg_joystick_x = RemoteXY.joystick_01_x;
    leg_joystick_y = RemoteXY.joystick_01_y;
    leg_button_R = RemoteXY.button_01;
    leg_button_L = RemoteXY.button_02;
    leg_button_shift = RemoteXY.pushSwitch_01;
    yagura_L = RemoteXY.button_03;
    yagura_R = RemoteXY.button_04;
    arm_joystick_x = RemoteXY.joystick_02_x;
    arm_joystick_y = RemoteXY.joystick_02_y;
    arm_button_UP = RemoteXY.button_05;
    arm_button_DOWN=RemoteXY.button_06;
    hand_button_UP = RemoteXY.button_07;
    hand_button_DOWN=RemoteXY.button_08;
    arm_button_init = RemoteXY.selectorSwitch_01 == 1;
    arm_button_pick = RemoteXY.selectorSwitch_01 == 2;
    arm_button_drop = RemoteXY.selectorSwitch_01 == 3;
  }
#elif defined(SERIAL_CONTROLLER)
  #include "Rimocon_Serial.h"
  inline void SerialInput(){}
#endif

inline int sign(auto x){return (x>0)-(x<0);}
