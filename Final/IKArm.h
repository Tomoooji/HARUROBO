#pragma once

#include <PCA9685.h>
#include <Arduino.h>

constexpr int SERVOMAX = 470, SERVOMIN = 120; //     PCA9685に渡す周波数のレンジ(基本変更しない)
constexpr float arm_speed = 1;//1.25 //              アーム手首位置の移動速度[n*0.6cm(←目安)]
constexpr int wrist_speed = 3; //                    アーム手首角度の回転速度[degree]
constexpr int finger_speed = 6; //                   ハンドの回転速度[deg+ree]
constexpr int clow_speed = 6; //                     ワーク引きずる爪の速度[degree]

constexpr float arm_pos_init[] = { 10,  10, PI}; // アーム待機位置x,yと手首角度[radian]
constexpr float arm_pos_pick[] = {162.24/4, -89.44/4, radians(135)}; // ワーク拾う位置x,yと手首角度[radian]
constexpr float arm_pos_drop[] = {147.24/4, 152.36/4, 0.5}; // ワークセット位置x,yと手首角度[radian]

constexpr int ArmLength[] = {36, 26, 21}; // アーム関節長さ(3個目は不使用)
constexpr int InitalAngle[] = {0, 0, 90}; // アームのサーボ取付角度(完全折り畳み時が0°)


constexpr int finger_min = 40, finger_max = 140; // degrees
constexpr int clow_min = 40, clow_max = 140;

inline float clip2pi(float ang){
  return (ang<0? ang+TWO_PI: (ang>TWO_PI? ang-TWO_PI: ang));
}

struct IK2{
  const int* jointLength;
  int jointLength_sq[2];
  float wrist[2];
  float elbow[2];
  float jointAngle[2];
  float distance_sq;
  float distance;
  IK2(const int jointLen[]): jointLength(jointLen){
    this->jointLength_sq[0] = sq(this->jointLength[0]);
    this->jointLength_sq[1] = sq(this->jointLength[1]);
  }
};

inline void setWrist(IK2& ik, float x = arm_pos_init[0], float y = arm_pos_init[1]){
  ik.wrist[0] = x;
  ik.wrist[1] = y;
}

inline void calcAngle(IK2& ik){
  ik.distance_sq = sq(ik.wrist[0]) + sq(ik.wrist[1]); ik.distance = sqrt(ik.distance_sq);
  ik.jointAngle[0] = clip2pi(acos(constrain((
    (ik.distance_sq +ik.jointLength_sq[0] -ik.jointLength_sq[1]) / (2 *ik.distance *ik.jointLength[0])
  ),-1.0f,1.0f)) + atan2(ik.wrist[1], ik.wrist[0]) );
  ik.jointAngle[1] = clip2pi(acos(constrain((
    (ik.jointLength_sq[0] +ik.jointLength_sq[1] -ik.distance_sq) / (2 *ik.jointLength[0] *ik.jointLength[1])
  ),-1.0f,1.0f)));
}

inline void getWrist(IK2& ik, float& _result){
  _result[0] = 0.0f; _result[1] = 0.0f;
  float ang = 0.0;
  for(int i=0; i<2; i++){
    ang += i-PI;
    _result[0] += ik.armLength[i] *cos(ik.jointAngle[i]);
    _result[1] += ik.armLength[i] *sin(ik.jointAngle[i]);
  }
}

inline void ajastAngle(IK2& ik, int idx, float da){
  ik.jointAngle[idx] += da;
}

inline void InitIK(IK2& ik){
  setWrist(ik);
  calcAngle(ik);
}

inline bool isWristOK(IK2& ik){return ik.distance > ik.jointLength[0] + ik.jointLength[1] || ik.distance < ik.jointLength[0] - ik.jointLength[1];}

////////////////////////////////////////////////////////

class Arm{
 public:
  enum SERVOID{
    SHOULDER,
    ELBOW,
    WRIST,
    FINGER,
    CLOW
  };
  const int* initAngle;
  int servoAngle[5];
  IK2 Ik;
  PCA9685 pwm = PCA9685(0x40);
  Arm(const int Lengths[], const int Angles_Init[]): Ik(Lengths) {}
  
  void begin(bool isTest = false){
    if(!isTest){
      this->pwm.begin();
      this->pwm.setPWMFreq(50);
    }
    InitIK(this->Ik);
    for(int i=0; i<3; i++){
      this->servoAngle[i] = degrees(clip2pi(this->Ik.jointAngle[i] - this->initAngle[i]));
    }
  }
  
  bool moveWrist(float dx, float dy){
    setWrist(this->Ik,
             this->Ik.wrist[0] + constrain(dx, -100, 100),
             this->Ik.wrist[1] + constrain(dy, -100, 100));
    calcAngle(this->Ik);
    for(int i=0; i<2; i++){
      this->servoAngle[i] = degrees(clip2pi(this->Ik.jointAngle[i] - this->initAngle[i]));
    }
    return isWristOK(this->Ik);
  }
  
  void rotateHand(int da){
    this->servoAngle[2] = constrain(this->servoAngle[2] +da, 0, 180);
  }
  
  void moveFinger(int da){
    this->servoAngle[3] = constrain(this->servoAngle[3] +da, finger_min, finger_max);
  }
  void moveClow(int da){
    this->servoAngle[4] = constrain(this->servoAngle[4] +da, clow_min, clow_max);
  }
  
  void ajastIk(int idx, int da){
    this->Ik.jointAngle[idx] = radians(this->servoAngle[idx] +da) + this->initAngle[i];
    getWrist(this->Ik, this->Ik.wriet);
  }

  void updateServos(){
    // don't use for to ajast set direction
    this->pwm.setPWM(Channels[0],0,map(180-constrain(this->servoAngle[0], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
    this->pwm.setPWM(Channels[1],0,map(180-constrain(this->servoAngle[1], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
    this->pwm.setPWM(Channels[2],0,map(180-constrain(this->servoAngle[2], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
    this->pwm.setPWM(Channels[3],0,map(constrain(this->servoAngle[3], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
    this->pwm.setPWM(Channels[4],0,map(180-constrain(this->servoAngle[3], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
    this->pwm.setPWM(Channels[5],0,map(constrain(this->servoAngle[4], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
  }

};



