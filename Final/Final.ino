// 入力用変数 //
bool connection_flag;
int leg_joystick_x, leg_joystick_y;
bool leg_button_R, leg_button_L;
bool leg_button_shift;
bool yagura_R, yagura_L;
int arm_joystick_x, arm_joystick_y;
bool arm_button_UP, arm_button_DOWN;
bool arm_button_init, arm_button_pick, arm_button_drop;
bool hand_button_UP, hand_button_DOWN;

#define PS4_CONTROLLER // or REMOTEXY_BTCL or REMOTEXY_BLE or SERIAL_CONTROLLER
#include "SwitchMode.h"

// 出力用定数 //
constexpr int DC_default_speed = 200;
constexpr float leg_motor_gains[] = {1, 1, 1, 1};
constexpr int SERVOMAX = 470, SERVOMIN = 120;
constexpr float arm_speed = 1;//1.25
constexpr int wrist_speed = 3; //degree
constexpr int finger_speed = 6; //degree
constexpr int led_power = 150;

constexpr float arm_pos_init[] = {1.42, 0.33, 0.00}; //radian
constexpr float arm_pos_pick[] = {}; //radian
constexpr float arm_pos_drop[] = {}; //radian

//--環境依存定数--//
constexpr int DCpins[]={
//  FR     BR      BL      FL    yagura 
  16, 17, 2, 15, 14, 12, 26, 27, 25, 33
};
constexpr int Channels[] = {
  0, 2, 4, 6
};
constexpr int ArmLength[] = {36, 26, 21};
constexpr float InitalAngle[] = {
  radians(0),radians(0),radians(90)
};//{-30, 0, -30};

//--出力用変数--//
#include "OmuniLeg.h"
#include "IKArm.h"

float leg_speed = 1.0;
int _direcX, _direcY; // 触るべからず
Arm sakuarm(ArmLength, InitalAngle);

void setup(){
  Serial.begin(9600);

  //--init controller--//
  //PS4.begin(MAC_PS4CON);
  RemoteXY_Init();
  //Serial2.begin(9600,SERIAL_8N1, 16, 17);

  //--init DCmotors--//
  for(int pin: DCpins){
    ledcAttach(pin,12800,8);
    ledcWrite(pin,0);
  }

  //--init Arm--//
  sakuarm.begin(true);

  //--init LED--//
  for(int pin: LEDpins){
    ledcAttach(pin,12800,8);
    ledcWrite(pin,50);
  }
}

void loop(){
  //--get inputs--//
  //PS4Input();
  RemoteXYEngine.handler(); RemoteXYInput();
  //SerialInput();

  //--process logic--//
  if(connection_flag){
  //-manage omuni-//
    if(sq(leg_joystick_x) + sq(leg_joystick_y) > sq(range_ignoreLstick)){
      setdirection(atan2(leg_joystick_y, leg_joystick_x), _direcX, _direcY);
    //do another job if using PC controll...
    }else{
      _direcX = 0; _direcY = 0;
    } // →set direcX/Y
    //if(leg_button_shift)leg_speed = (leg_speed==1.0? 0.5:1.0);
    driveomuni(_direcX, _direcY, leg_button_R-leg_button_L,255*leg_speed);

  //-manage yagura-//
    drivemotor(YAGURAARM, 200*(yagura_L - yagura_R));

  //-manage ikarm-//
    if(sq(arm_joystick_x) + sq(arm_joystick_y) > sq(range_ignoreRstick+10)){
      sakuarm.moveWrist(arm_speed *0.01*(arm_joystick_x), // should be changed about  arm_speed*0.01*..
                        arm_speed *0.01*(arm_joystick_y));//
    }
    sakuarm.rotateHand(wrist_speed*(arm_button_UP - arm_button_DOWN));
    sakuarm.moveFinger(hand_speed*(hand_button_UP - hand_button_DOWN));
    sakuarm.updateServos();

  }else{

  }
  //*------------------
  Serial.print(sakuarm.servoAngle[0]); Serial.print(",");
  Serial.print(sakuarm.servoAngle[1]); Serial.print(",");
  Serial.print(sakuarm.servoAngle[2]); Serial.print(",");
  Serial.print(sakuarm.Ik.wrist[0]); Serial.print(",");
  Serial.println(sakuarm.Ik.wrist[1]);
  //*///-----------------
  ledcWrite(LEDpins[0],connection_flag*led_power);

  //delay(10);
  RemoteXYEngine.delay(50);
}





