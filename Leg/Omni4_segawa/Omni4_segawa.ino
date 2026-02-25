//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    



// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLUETOOTH

#include <BluetoothSerial.h>



// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 47 bytes V19 
  { 255,4,0,0,0,40,0,19,0,0,0,0,31,1,106,200,1,1,3,0,
  5,5,35,92,92,32,2,26,31,1,10,13,24,24,0,2,31,0,1,71,
  12,24,24,0,2,31,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  uint8_t leftturn; // =1 if button pressed, else =0, from 0 to 1
  uint8_t rightturn; // =1 if button pressed, else =0, from 0 to 1

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   

#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



int speed1, speed2;
float theta1, radius1;
//         Y          G
int pin2 = 16, pin1 = 17; // front right
int pin4 = 12, pin3 = 14; // front left
int pin5 = 0,  pin6 = 4;  // back left
int pin8 = 15, pin7 = 2;  // back right

//void calcomni(int x, int y);
//void drivemotor(int speed, int pin1, int pin2);



float roundRad(float angle){
  return round(angle*100)/100.0f;
}
void calcomni(int x, int y) {
  radius1 = sqrt(sq(x) + sq(y));
  radius1 = radius1 > 100 ? 100 : radius1;
  theta1 = roundRad(0.25 * PI + atan2(y, x));
  speed1 = cos(theta1) * (radius1 * 2.55);
  speed2 = sin(theta1) * (radius1 * 2.55);
}

void drivemotor(int speed, int pinA, int pinB) {
  //analogWrite(pin1, HIGH);
  //analogWrite(pin2, LOW);
  int pwm = abs(speed);
  if (pwm > 255) pwm = 255;

  if (speed > 10) {
    // 前転
    ledcWrite(pinA, pwm);
    ledcWrite(pinB, 0);
  }
  else if (speed < -10) {
    // 後転
    ledcWrite(pinA, 0);
    ledcWrite(pinB, pwm);
  }
  else {
    // 停止
    ledcWrite(pinA, 0);
    ledcWrite(pinB, 0);
  }
}

int filter(int val, int limit=15){
  return abs(val)<limit? 0: val;
}

void turning(int pina, int pinb, int dir) {
  if (dir == 1) {
    // 正転
    ledcWrite(pina, 100);
    ledcWrite(pinb, 0);
  } else if (dir == -1) {
    // 逆転
    ledcWrite(pina, 0);
    ledcWrite(pinb, 100);
  } else {
    // 停止
    ledcWrite(pina, 0);
    ledcWrite(pinb, 0);
  }
  RemoteXYEngine.delay(1);
}



void setup() 
{
  RemoteXY_Init ();  // initialization by macros 
  ledcAttach(pin1, 12800, 8);
  ledcAttach(pin2, 12800, 8);
  ledcAttach(pin3, 12800, 8);
  ledcAttach(pin4, 12800, 8);
  ledcAttach(pin5, 12800, 8);
  ledcAttach(pin6, 12800, 8);
  ledcAttach(pin7, 12800, 8);
  ledcAttach(pin8, 12800, 8);

  
  // TODO you setup code
  
}

void loop() 
{ 
  RemoteXYEngine.handler ();
  if (RemoteXY.leftturn == 1) {
    // 左旋回: すべての車輪を同じ方向に回す（配置に合わせて調整が必要）
    turning(pin2, pin1, 1);
    turning(pin4, pin3, -1);
    turning(pin5, pin6, 1);
    turning(pin7, pin8, -1);
  }
  else if (RemoteXY.rightturn == 1) {
    // 右旋回
    turning(pin2, pin1, -1);
    turning(pin4, pin3, 1);
    turning(pin5, pin6, -1);
    turning(pin7, pin8, 1);
  } else {   
  //calcomni(RemoteXY.joystick_01_x, RemoteXY.joystick_01_y);
  //RemoteXY_Handler();   

  calcomni(filter(RemoteXY.joystick_01_x),
           filter(RemoteXY.joystick_01_y));

  // 向かい合う2輪に同じ成分を与える
  // cos成分 → 右前(fr) & 左後ろ(bl)
  // sin成分 → 左前(fl) & 右後ろ(br)

  drivemotor(speed1, pin1, pin2);   // fr  = r'cosθ'
  drivemotor(speed2, pin3, pin4);   // fl  = r'sinθ'
  drivemotor(speed1, pin5, pin6);   // bl  = r'cosθ'
  drivemotor(speed2, pin7, pin8);   // br  = r'sinθ'
  }
  RemoteXYEngine.delay(100);
  
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead RemoteXYEngine.delay() 

}