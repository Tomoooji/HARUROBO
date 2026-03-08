#pragma pack(1)
struct Payload{
  uint16_t header = 0x55AA;
  uint16_t buttons;
  int16_t val;
} data;
#pragma pack()

constexpr int centerx = 512, centery = 512;
constexpr int limit = 50;
constexpr float range_othogonal = radians(25);
float inputAngle;
/*
bool leg_button_R;
bool leg_button_L;
bool leg_button_shift;
bool yagura_R;
bool yagura_L;
bool arm_zplus;
bool arm_zminus;
bool arm_yplus;
bool arm_yminus;
 //int arm_joystick_x, arm_joystick_y;
bool arm_button_UP;
bool arm_button_DOWN;
bool arm_button_init;
bool arm_button_pick;
bool arm_button_drop;
bool finger_button_UP;
bool finger_button_DOWN;
int leg_direc;
  //int leg_joystick_x;
  //int leg_joystick_y;
*/

uint8_t direc(int x, int y){
  if(sq(centerx-x)+sq(centery-y) > sq(limit)){
    inputAngle = atan2(centery-y, centerx-x); //form -PI to PI
    if(inputAngle > PI -range_othogonal || inputAngle < -PI +range_othogonal){
      // LEFT
      return 1;
    }
    else if(inputAngle <= -0.5*PI -range_othogonal){
      // BACKLEFT
      return 2;
    }
    else if(inputAngle < -0.5*PI +range_othogonal){
      // BACK
      return 3;
    }
    else if(inputAngle <= 0 -range_othogonal){
      // BACKRIGHT
      return 4;
    }
    else if(inputAngle < 0 +range_othogonal){
      // RIGHT
      return 5;
    }
    else if(inputAngle <= 0.5*PI -range_othogonal){
      // FRONTRIGHT
      return 6;
    }
    else if(inputAngle < 0.5*PI +range_othogonal){
      // FRONT
      return 7;
    }
    else if(inputAngle <= PI -range_othogonal){
      // FRONTLEFT
      return 8;
    }
    else{
      return 0;
    }
  }
  else{
    return 0;
  }
}

constexpr uint8_t RXD2 = 16, TXD2 = 17;
uint8_t digitalpin[] = {// 16pinいる
  15, 2, 0, 4, 5, 18, 19, 23, //16, 17, 21, 22   8pin
  13, 12, 14, 27, 26, 25, 34, 35//33, 32         8pin
};
uint8_t analogpin[] = {
  33, 32
};

void setup(){
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  for(uint8_t pin: digitalpin){
    pinMode(pin, pin>=34? INPUT: INPUT_PULLUP);
  }
  pinMode(21, OUTPUT);
}

void loop(){
  for(int i=0; i<16; i++){
    if(digitalRead(digitalpin[i]) == LOW){
      data.buttons |= (1 << i);
    }
  }
  data.val = direc(analogRead(analogpin[0]),
                   analogRead(analogpin[1]));
  Serial2.write((const uint8_t*)&data, sizeof(data));
  
  digitalWrite(21,digitalRead(digitalpin[4]));

  delay(10);
}



/*

  for(uint8_t pin: digitalpin){
    Serial2.print(!digitalRead(pin));
    Serial2.print(",");
  }
  Serial2.print(analogRead(analogpin[0])); Serial.print(",");
  Serial2.print(analogRead(analogpin[1])); Serial.println();
  delay(10);
*/



