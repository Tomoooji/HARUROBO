#pragma once

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