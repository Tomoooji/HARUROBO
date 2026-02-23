//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
//#define REMOTEXY_MODE__ESP32CORE_BLE
#define REMOTEXY_MODE__ESP32CORE_BLUETOOTH

//#include <BLEDevice.h>
#include <BluetoothSerial.h>

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "あああ"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 29 bytes V19 
  { 255,2,0,0,0,22,0,19,0,0,0,0,31,1,106,200,1,1,1,0,
  5,27,18,60,60,32,2,26,31 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   

#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


int speed1, speed2;
float theta1, radius1;
int pin1 = 15, pin2 = 2; // front right
int pin3 = 0, pin4 = 4; // front left
int pin5 = 16, pin6 = 17; // back left
int pin7 = 18, pin8 = 5; // back right

void calcomni(int x, int y);
void drivemotor(int speed, int pin1, int pin2);

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
  calcomni(RemoteXY.joystick_01_x, RemoteXY.joystick_01_y);
  RemoteXY_Handler();   

  calcomni(RemoteXY.joystick_01_x, RemoteXY.joystick_01_y);

  // 向かい合う2輪に同じ成分を与える
  // cos成分 → 右前(fr) & 左後ろ(bl)
  // sin成分 → 左前(fl) & 右後ろ(br)

  drivemotor(speed1, pin1, pin2);   // fr  = r'cosθ'
  drivemotor(speed2, pin3, pin4);   // fl  = r'sinθ'
  drivemotor(speed1, pin5, pin6);   // bl  = r'cosθ'
  drivemotor(speed2, pin7, pin8);   // br  = r'sinθ'

  RemoteXYEngine.delay(100);
  
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead RemoteXYEngine.delay() 

}

void calcomni(int x, int y) {
  radius1 = sqrt(sq(x) + sq(y));
  radius1 = radius1 > 100 ? 100 : radius1;
  theta1 = 0.25 * PI + atan2(y, x);
  speed1 = cos(theta1) * (radius1 * 2.55);
  speed2 = sin(theta1) * (radius1 * 2.55);
}

void drivemotor(int speed, int pin1, int pin2) {
  //analogWrite(pin1, HIGH);
  //analogWrite(pin2, LOW);
  int pwm = abs(speed);
  if (pwm > 255) pwm = 255;

  if (speed > 10) {
    // 前転
    ledcWrite(pin1, pwm);
    ledcWrite(pin2, 0);
  }
  else if (speed < -10) {
    // 後転
    ledcWrite(pin1, 0);
    ledcWrite(pin2, pwm);
  }
  else {
    // 停止
    ledcWrite(pin1, 0);
    ledcWrite(pin2, 0);
  }
}