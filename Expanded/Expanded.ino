// モード選択 //
#define RemoteXY_BLE
//#define RemoteXY_BTCL
//#define PS4_RIMOCON

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


// 入力用変数 //
bool connection_flag;
int leg_joystick_x, leg_joystick_y;
bool leg_button_R, leg_button_L;
bool yagura_R, yagura_L;

int arm_joystick_x, arm_joystick_y;
bool arm_button_UP, arm_button_DOWN;
bool arm_button_init, arm_button_chatch, arm_button_shoot;

bool hand_button_UP, hand_button_DOWN;

// ファイルのinclude //
#include "SerectMode.h"
#include "Omuni.h"
#include "InverseKinematics.h"

// 内部変数 //
int _direcX, _direcY; // 触るべからず
IK3 arm=IK3(ArmLength, InitalAngle);
int _handAng = 90;

void setup(){
  Serial.begin(9600);/////
  
  //PS4.begin(MAC_PS4CON);
  RemoteXY_Init();

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
  //PS4Input();
  RemoteXYEngine.handler(); RemoteXYInput();
  
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
    //arm.servoAngle[2] += wrist_speed*(arm_button_UP - arm_button_DOWN);
    arm.servoAngle[2] = constrain(arm.servoAngle[2] + +wrist_speed*(arm_button_UP - arm_button_DOWN),0,PI);
    // →wrist angle
    
    _handAng = constrain(_handAng + hand_speed*(hand_button_UP - hand_button_DOWN), hand_min, hand_max);
    //　→hand(finger) angle
    
    if(arm_button_init){
      arm.servoAngle[0] = ;
      arm.servoAngle[0] = ;
      arm.servoAngle[0] = ;
    }

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

  //delay(10);
  RemoteXYEngine.delay(50);
}