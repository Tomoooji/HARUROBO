#define BLE//or BTCL

#include "RemoteXYinputs.h"
#include <PCA9685.h>
#include "InverseKinematics.h"

//サーボ用の設定//
#define PWMFreq 50
#define SERVOMIN 95
#define SERVOMAX 605
constexpr uint8_t Chanels[3] = {15,13,11}; // 各サーボを接続するPCAのチャンネル
PCA9685 pwm = PCA9685(0x40); // こいつ使う

//逆運動学計算用の設定//
constexpr int ArmLength[3] = {36, 26, 21};
constexpr float InitalAngle[3] = {0,0,0};//{-30, 0, -30};
IK3 arm = { // こいつ使う
  ArmLength,
  InitalAngle // 残りは0で初期化される
};

const float rGain=0.01*PI;

void setup() {
  RemoteXY_Init();
  pwm.begin();
  pwm.setPWMFreq(PWMFreq);
  InitIK(arm);
  Serial.begin(9600); // デバックとシミュレータ用
}

void loop() {
  //入力//
  RemoteXYEngine.handler();
  //判断//
  if(RemoteXY.led_01=RemoteXY.connect_flag){
    moveTarget(arm,
               filter(RemoteXY.joystick_01_x),
               filter(RemoteXY.joystick_01_y),
               (RemoteXY.button_05-RemoteXY.button_06)*rGain);
    calcAngle(arm);
    //出力//
    radmoveServo(3,Chanels,arm.servoAngle);
    checkinf();
  }
}

int filter(int8_t val){
  return (val>20 ? 1 : (val<-20? -1 : 0));
}

void radmoveServo(int num, const uint8_t* ch, const float* radAng){
  for(int i=0;i<num;i++){
    pwm.setPWM(ch[i],0,map(constrain(degrees(radAng[i]),10,170),0,180,SERVOMAX,SERVOMIN));
    //*
    Serial.print(degrees(radAng[i]));
    if(i<num-1){
      Serial.print(",");
    }else{
      Serial.println();
    }
    //*/
    RemoteXYEngine.delay(10);
    //delay(10);
  }
}

void checkinf(){
  RemoteXY.instrument_01=degrees(arm.servoAngle[0]);
  RemoteXY.instrument_02=degrees(arm.servoAngle[1]);
  RemoteXY.instrument_03=degrees(arm.servoAngle[2]);
  RemoteXY.linearbar_01=arm.x;
  RemoteXY.linearbar_02=arm.y;
  RemoteXY.instrument_04=degrees(arm.a);
  if(arm.servoAngle[0]>PI || arm.servoAngle[1]>PI || arm.servoAngle[2]>PI){
    RemoteXY.led_02=1;
  }else{
    RemoteXY.led_02=0;
  }
}