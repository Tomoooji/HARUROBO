//--入力用変数--//
bool connection_flag; //                      接続確認用
bool disconect_button;
int leg_joystick_x, leg_joystick_y; //        足回り方向指定
int leg_direc; //                             足回り方向指定(ジョイスティック以外のUI用)
bool leg_button_R, leg_button_L; //           旋回方向
bool leg_button_shift; //                     低速走行
bool yagura_R, yagura_L; //                   櫓用アーム開閉
int arm_joystick_x, arm_joystick_y; //        アーム手首位置操作
bool arm_zplus, arm_zminus, arm_yplus, arm_yminus;
  //                                          アーム手首位置操作(ジョイスティック以外のUI用)
bool arm_button_UP, arm_button_DOWN; //       アーム手首角度調節
bool arm_button_init, arm_button_pick, arm_button_drop;
  //                                          アームの指定位置コマンド
bool finger_button_UP, finger_button_DOWN; // ハンドの開閉
bool clow_button_UP, clow_button_DOWN; //     ワークを引きずるやつ
//bool appeal_button;

//PS4_CONTROLLER or REMOTEXY_BTCL or REMOTEXY_BLE or SERIAL_CONTROLLER
#define REMOTEXY_BLE
#include "SwitchMode.h"

//--出力用定数--//
constexpr int DC_default_speed = 150; //             櫓用アームの回転速度(足回りは255)[n/255]
constexpr float leg_motor_gains[] = {1, 1, 1, 1}; // 足回りモーターの速度定数[最大のn倍]
constexpr int SERVOMAX = 470, SERVOMIN = 120; //     PCA9685に渡す周波数のレンジ(基本変更しない)
constexpr float arm_speed = 1;//1.25 //              アーム手首位置の移動速度[n*0.6cm(←目安)]
constexpr int wrist_speed = 2; //                    アーム手首角度の回転速度[degree]
constexpr int finger_speed = 3; //                   ハンドの回転速度[degree]
constexpr int clow_speed = 3;
constexpr int led_power = 150; //                    動作チェック用LEDの強さ
// 45.08/4, 80.88/4
constexpr float arm_pos_init[] = { 10,  10, PI}; // アーム待機位置x,yと手首角度[degree]
constexpr float arm_pos_pick[] = {162.24/4, -85.44/4, 2.72}; // ワーク拾う位置x,yと手首角度[degree]
constexpr float arm_pos_drop[] = {147.24/4, 182.36/4, 0.5}; // ワークセット位置x,yと手首角度[degree]

//--環境依存定数--//
constexpr int DCpins[]={ // IBT_2用信号線のピン
//  FR      BR      BL      FL    yagura 
   2, 15, 14, 12, 26, 27, 18,  5, 25, 33
};
constexpr int Channels[] = { // PCA9685の使用チャンネル(根本→手先順)
  0, 2, 4, 6, 8
};
constexpr int ArmLength[] = {36, 26, 21}; // アーム関節長さ(3個目は不使用)
constexpr int InitalAngle[] = {0, 0, 90}; // アームのサーボ取付角度(完全折り畳み時が0°)
constexpr int LEDpins[]={32, 4, 0}; //       動作チェック用LEDのピン(接続,アーム閾値,低速走行)

//--出力用変数--//
#include "OmuniLeg.h"
#include "IKArm.h"

bool leg_slow = true;
int _direcX, _direcY; // 触るべからず
Arm sakuarm(ArmLength, InitalAngle);

void setup(){
  Serial.begin(9600);

  //--init controller--//
  //PS4.begin(MAC_PS4CON);
  RemoteXY_Init();
  ////Serial2.begin(115200,SERIAL_8N1, 16, 17);

  //--init DCmotors--//
  for(int pin: DCpins){
    ledcAttach(pin,12800,8);
    ledcWrite(pin,0);
  }

  //--init Arm--//
  sakuarm.begin(1);//true

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
  //connection_flag = SerialInput();

  //--process logic--//
  if(disconnect_button && connection_flag) connection_flag = !connection_falg;
  if(connection_flag){
  //-manage omuni-//
    if(sq(leg_joystick_x) + sq(leg_joystick_y) > sq(range_ignoreLstick)){
      setdirection(atan2(leg_joystick_y, leg_joystick_x), _direcX, _direcY);
    }
    else{
      _direcX = 0; _direcY = 0;
    } // →set direcX/Y

    /*if(leg_button_shift){
      leg_slow = !leg_slow;
      ledcWrite(LEDpins[2], leg_slow *led_power);
      //Serial.println(leg_slow);
    }*/
    ledcWrite(LEDpins[2], (leg_slow = leg_button_shift) *led_power);

    driveomuni(_direcX, _direcY, leg_button_R-leg_button_L, 255 *(!leg_slow+0.5));

  //-manage yagura-//
    drivemotor(YAGURAARM, DC_default_speed *(yagura_L - yagura_R));

  //-manage ikarm-//
    if(arm_button_init){
      setWrist(sakuarm.Ik, arm_pos_init[0], arm_pos_init[1]);
      sakuarm.servoAngle[2] = degrees(arm_pos_init[2]);
    }
    if(arm_button_pick){
      setWrist(sakuarm.Ik, arm_pos_pick[0], arm_pos_pick[1]);
      sakuarm.servoAngle[2] = degrees(arm_pos_pick[2]);
    }
    if(arm_button_drop){
      setWrist(sakuarm.Ik, arm_pos_drop[0], arm_pos_drop[1]);
      sakuarm.servoAngle[2] = degrees(arm_pos_drop[2]);
    }
    if(sq(arm_joystick_x) + sq(arm_joystick_y) > sq(range_ignoreRstick+10)){
      if(sakuarm.moveWrist(arm_speed *0.01*(arm_joystick_x), // should be changed about  arm_speed*0.01*..
                        arm_speed *0.01*(arm_joystick_y))){//
        ledcWrite(LEDpins[1],led_power);
      }else{
        ledcWrite(LEDpins[1],0);
      }
    }else{
      sakuarm.moveWrist(0,0);
    }
    sakuarm.rotateHand(wrist_speed*(arm_button_UP - arm_button_DOWN));
    sakuarm.moveFinger(finger_speed*(finger_button_UP - finger_button_DOWN));
    sakuarm.moveClow(clow_speed*(clow_button_UP - clow_button_DOWN));
    //sakuarm.updateServos();

  }else{
    for(int pin: DCpins){
      ledcWrite(pin,0);
    }
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
  RemoteXYEngine.delay(10);
}




