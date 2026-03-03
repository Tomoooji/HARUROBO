// ライブラリのinclude //
#include <PS4Controller.h>
//#include "RemoteXYrimocon.h"

// 定義 //
constexpr int FRONTRIGHT = 0;
constexpr int BACKRIGHT  = 1;
constexpr int BACKLEFT   = 2;
constexpr int FRONTLEFT  = 3;
constexpr int YAGURAARM  = 4;

// 定数 //
constexpr int DC_default_speed = 200;
constexpr float range_othogonal = radians(25); // 前後左右に±50°,斜めは±40°
constexpr int range_ignoreLstick = 40;
constexpr int line_RL2pushed = 50;

constexpr int range_ignoreRstick = 20;
constexpr float arm_speed = 1.25;
constexpr float wrist_speed = 0.05; // radians
constexpr int hand_speed = 6; // degrees
constexpr int hand_min = 40, hand_max = 140; // degrees

constexpr int led_power = 150;

// 環境依存の定数 //
constexpr char MAC_PS4CON[] = "e4:65:b8:d8:d4:80";
constexpr int DCpins[]={
  16, 17, 2, 15, 14, 12, 26, 27, 25, 33
};

constexpr int ArmLength[3] = {36, 26, 21};
constexpr float InitalAngle[3] = {radians(0),radians(0),radians(90)};//{-30, 0, -30};

constexpr int LEDpins[]={32};
// connection, anglecorrect, dcpowered(これは回路側で実装？)

// ファイルのinclude //
#include "Omuni.h"
#include "InverseKinematics.h"
////extern PCA9685 servo_pwm;

// 入力用変数 //
bool connection_flag;
int leg_joystick_x, leg_joystick_y;
bool leg_button_R, leg_button_L;
bool yagura_R, yagura_L;

int arm_joystick_x, arm_joystick_y;
bool arm_button_UP, arm_button_DOWN;
////bool arm_button_init, arm_button_chatch, arm_button_shoot;

bool hand_button_UP, hand_button_DOWN;

// 内部変数 //
int _direcX, _direcY; // 触るべからず
IK3 arm=IK3(ArmLength, InitalAngle);
int _handAng = 90;

// update controller values
void PS4Input(){
  connection_flag = PS4.isConnected();
  
  leg_joystick_x = PS4.LStickX();
  leg_joystick_y = PS4.LStickY();
  leg_button_R = PS4.R2Value() > line_RL2pushed;
  leg_button_L = PS4.L2Value() > line_RL2pushed;
  
  yagura_L = PS4.Left();
  yagura_R = PS4.Right();

  arm_joystick_x = PS4.RStickX();
  arm_joystick_y = PS4.RStickY();
  arm_button_UP =PS4.Circle();
  arm_button_DOWN =PS4.Cross();
  
  hand_button_UP = PS4.L1();
  hand_button_DOWN=PS4.R1();
}
/*
void RemoteXYInput(){
  connection_flag = RemoteXY.connect_flag;
  leg_joystick_x = RemoteXY.joystick_01_x;
  leg_joystick_y = RemoteXY.joystick_01_y;
  leg_button_R = RemoteXY.button_01;
  leg_button_L = RemoteXY.button_02;
  yagura_L = RemoteXY.button_03;
  yagura_R = RemoteXY.button_04;
  arm_joystick_x = RemoteXY.joystick_02_x;
  arm_joystick_y = RemoteXY.joystick_02_y;
  arm_button_UP = RemoteXY.button_05;
  arm_button_DOWN=RemoteXY.button_06;
  hand_button_UP = RemoteXY.button_07;
  hand_button_DOWN=RemoteXY.button_08;
}*/

void setup(){
  Serial.begin(9600);/////
  PS4.begin(MAC_PS4CON);
  //RemoteXY_Init();

  InitIK(arm);

  // init servo motors //
  servo_pwm.begin();
  servo_pwm.setPWMFreq(50);

  // init DCmotors //
  for(int pin: DCpins){
    ledcAttach(pin,12800,8);
    ledcWrite(pin,0);
  }
  for(int pin: LEDpins){
    ledcAttach(pin,12800,8);
    ledcWrite(pin,0);
  }
}

void loop(){
  PS4Input();
    //RemoteXYEngine.handler();
    //RemoteXYInput();
  ledcWrite(LEDpins[0],connection_flag*led_power);
  if(connection_flag){
 
    //-- manage omuni --//
    if(sq(leg_joystick_x) + sq(leg_joystick_y) > sq(range_ignoreLstick)){
      setdirection(atan2(leg_joystick_y, leg_joystick_x), _direcX, _direcY);
    }else{
      _direcX = 0; _direcY = 0;
    } // →set direcX/Y
    driveomuni(_direcX, _direcY, leg_button_R-leg_button_L,255);

    //-- manege yaguraarm --//
    drivemotor(YAGURAARM, 200*(yagura_L - yagura_R));
    
    //-- manege ikarm --//
    if(sq(arm_joystick_x) + sq(arm_joystick_y) > sq(range_ignoreRstick)){
      moveTarget(arm, arm_speed*(sign(arm_joystick_x)), // dx
                      arm_speed*(sign(arm_joystick_y)));// dy
      calcAngle(arm);
    }
    // →arm(sholder&elbow) angle
    
    ///////moveWrist(arm, wrist_speed*(arm_button_UP - arm_button_DOWN));// dang
    arm.servoAngle[2] += wrist_speed*(arm_button_UP - arm_button_DOWN);
    // →wrist angle
    
    _handAng = constrain(_handAng + hand_speed*(hand_button_UP - hand_button_DOWN), hand_min, hand_max);
    //　→hand(finger) angle
    
    updateservo(arm, _handAng);//, Channels);// handだけdegreesだけど許してちょ♡
    // →apply angles
    
    ////////
    Serial.print(degrees(arm.servoAngle[0])); Serial.print(",");
    Serial.print(degrees(arm.servoAngle[1])); Serial.print(",");
    Serial.println(degrees(arm.servoAngle[2]));// Serial.print(",");
    //Serial.println(_handAng);
    //////////

  }else{
    for(int pin: DCpins){
      ledcWrite(pin,100);
    }
    Serial.println();
  }

  delay(10);
    //RemoteXYEngine.delay(50);
}