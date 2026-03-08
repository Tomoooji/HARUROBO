#pragma once

int inputvals[10];

String line;

bool SerialInput(){
  if(Serial.available()){
    line = Serial.readStringUntil('\n');
    if(sscanf(line.c_str(),
        "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
        &inputvals[0],&inputvals[1],&inputvals[2],&inputvals[3],&inputvals[4]],
        &inputvals[5],&inputvals[6],&inputvals[7],&inputvals[8],&inputvals[9]
        ) = 10){
      leg_joystick_x = inputvals[0]*10;
      leg_joystick_y = inputvals[1]*10;
      leg_button_R = inputvals[2];
      leg_button_L = inputvals[3];
      yagura_button_L = inputvals[4] > 0;
      yagura_button_R = inputvals[4] < 0;
      arm_joystick_x = inputvals[5]*100;
      arm_joystick_y = inputvals[6]*100;
      arm_button_UP =    inputvals[7] > 0;
      arm_button_DOWN =  inputvals[7] < 0;
      finger_button_UP =   inputvals[8] > 0;
      finger_button_DOWN = inputvals[8] < 0;
      arm_button_init = (inputvals[9] == 1);
      arm_button_pick = (inputvals[9] == 2);
      arm_button_drop = (inputvals[9] == 3);
      return true;
    }
  }
  leg_joystick_x = 0;
  leg_joystick_y = 0;
  leg_button_R = false;
  leg_button_L = false;
  yagura_button_L = false;
  yagura_button_R = false;
  arm_joystick_x = 0;
  arm_joystick_y = 0;
  arm_button_UP = false;
  arm_button_DOWN = false;
  finger_button_UP = false;
  finger_button_DOWN = false;
  arm_button_init = false;
  arm_button_pick = false;
  arm_button_drop = false;

  return false;
}

/*
#pragma pack(1)
struct Payload{
  uint16_t header = 0x55AA;
  uint16_t buttons;
  int16_t val;
} data;
#pragma pack()

const size_t PACKET_SIZE = sizeof(data);
uint8_t head;

inline bool SerialInput(){
  if(Serial2.available() >= PACKET_SIZE){
    Serial2.readBytes((uint8_t*)&head, 2);
    Serial.println(head);
    if(head == 0xAA55){
      data.header = head;
      Serial2.readBytes((uint8_t*)&data+2, PACKET_SIZE-2);
      leg_button_R = (data.buttons >> 0) & 1;
      leg_button_L = (data.buttons >> 1) & 1;
      leg_button_shift = (data.buttons >> 2) & 1;
      yagura_R = (data.buttons >> 3) & 1;
      yagura_L = (data.buttons >> 4) & 1;
      arm_zplus  = (data.buttons >> 5) & 1;
      arm_zminus = (data.buttons >> 6) & 1;
      arm_yplus  = (data.buttons >> 7) & 1;
      arm_yminus = (data.buttons >> 8) & 1;
      //int arm_joystick_x, arm_joystick_y;
      arm_button_UP   = (data.buttons >> 9) & 1;
      arm_button_DOWN = (data.buttons >> 10) & 1;
      arm_button_init = (data.buttons >> 11) & 1;
      arm_button_pick = (data.buttons >> 12) & 1;
      arm_button_drop = (data.buttons >> 13) & 1;
      finger_button_UP   = (data.buttons >> 14) & 1;
      finger_button_DOWN = (data.buttons >> 15) & 1;
      leg_direc = data.val;
      return true;
    }
  }else{
    Serial2.read();
  }
    return false;
}
*/