inline int sign(auto x){return (x>0)-(x<0);}

// モード選択 //
#define RemoteXY_BLE
//#define RemoteXY_BTCL
//#define PS4_RIMOCON

// 定義 //

// 定数 //

//constexpr float arm_speed = 1;//1.25;
//constexpr float wrist_speed = 0.05; // radians
//constexpr int hand_speed = 6; // degrees
//constexpr int hand_min = 40, hand_max = 140; // degrees
constexpr int range_ignoreRstick = 20;////

constexpr int led_power = 150;

// 環境依存の定数 //
constexpr char MAC_PS4CON[] = "e4:65:b8:d8:d4:80";//
constexpr int Channels[] = {
  0, 2, 4, 6
};
constexpr int LEDpins[]={32};// →connection, anglecorrect, dcpowered(これは回路側で実装？)

constexpr int ArmLength[3] = {36, 26, 21};
constexpr int InitalAngle[3] = {0, 0, 90};//{-30, 0, -30};


// 入力用変数 //
bool connection_flag;

int arm_joystick_x, arm_joystick_y;
bool arm_button_UP, arm_button_DOWN;
bool arm_button_init, arm_button_chatch, arm_button_shoot;
bool hand_button_UP, hand_button_DOWN;




// ファイルのinclude //
#include "SerectMode.h"
//#include "Omuni.h"
#include "InverseKinematics.h"

// 内部変数 //

Arm sakuarm(ArmLength, InitalAngle);


void setup(){
  Serial.begin(9600);
  
  //PS4.begin(MAC_PS4CON);
  RemoteXY_Init();

  sakuarm.begin(true);
  // init servo motors //

  // init DCmotors //
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
    //-- manege yaguraarm --//
    //-- manege ikarm --//
    if(sq(arm_joystick_x) + sq(arm_joystick_y) > sq(range_ignoreRstick+10)){
      sakuarm.moveWrist(arm_speed *0.01*(arm_joystick_x),
                        arm_speed *0.01*(arm_joystick_y));
    }
    sakuarm.rotateHand(wrist_speed*(arm_button_UP - arm_button_DOWN));
    sakuarm.moveFinger(hand_speed*(hand_button_UP - hand_button_DOWN));
    ////if(sq(arm_joystick_x) + sq(arm_joystick_y) > sq(range_ignoreRstick)){
    ////  moveTarget(arm, arm_speed*(sign(arm_joystick_x)), // dx
    ////                  arm_speed*(sign(arm_joystick_y)));// dy
    ////  calcAngle(arm);
    ////}
    // →arm(sholder&elbow) angle
    
    ///////moveWrist(arm, wrist_speed*(arm_button_UP - arm_button_DOWN));// dang
    //arm.servoAngle[2] += wrist_speed*(arm_button_UP - arm_button_DOWN);
    /////arm.servoAngle[2] = constrain(arm.servoAngle[2] +wrist_speed*(arm_button_UP - arm_button_DOWN),0,PI);
    // →wrist angle
    
    ////_handAng = constrain(_handAng + hand_speed*(hand_button_UP - hand_button_DOWN), hand_min, hand_max);
    // →hand(finger) angle    

    //updateservo(arm, _handAng);//, Channels);// handだけdegreesだけど許してちょ♡
    // →apply angles
    
    /////////
    Serial.print(sakuarm.servoAngle[0]); Serial.print(",");
    Serial.print(sakuarm.servoAngle[1]); Serial.print(",");
    Serial.print(sakuarm.servoAngle[2]); Serial.print(",");
    Serial.print(sakuarm.Ik.wrist[0]); Serial.print(",");
    Serial.println(sakuarm.Ik.wrist[1]);// Serial.print(",");
    //Serial.println(_handAng);
    //*////////

  }else{
    Serial.println();
  }

  //delay(10);
  RemoteXYEngine.delay(50);
}