#pragma once

#include <PCA9685.h>

#include <Arduino.h>

constexpr int Pos_Init[2] = {5, 10};
constexpr int hand_min = 40, hand_max = 140; // degrees

inline float clip2pi(float ang){
  return (ang<0? ang+TWO_PI: (ang>TWO_PI? ang-TWO_PI: ang));
}

struct IK2{
  const int* jointLength;
  const int* initAngle;
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

inline void setWrist(IK2& ik, float x = Pos_Init[0], float y = Pos_Init[1]){
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

inline void InitIK(IK2& ik){
  setWrist(ik);
  calcAngle(ik);
}

inline bool isWristOK(IK2& ik){return ik.distance > ik.jointLength[0] + ik.jointLength[1];}

////////////////////////////////////////////////////////
/*
#define SERVOMIN 120
#define SERVOMAX 470

constexpr float arm_speed = 1;//1.25;
constexpr float wrist_speed = degrees(0.05); // radians
constexpr int hand_speed = 6; // degrees
constexpr int hand_min = 40, hand_max = 140; // degrees
*/

class Arm{
 public:
  const int* initAngle;
  int servoAngle[4];
  IK2 Ik;
  PCA9685 pwm = PCA9685(0x40);
  Arm(const int Lengths[], const int Angles_Init[]):
    initAngle(Angles_Init), Ik(Lengths) {}
  
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
  
  void rotateHand(int da){// wrist_speed*(arm_button_UP - arm_button_DOWN)
    this->servoAngle[2] = constrain(this->servoAngle[2] +da, 0, 180);
  }
  
  void moveFinger(int da){// hand_speed*(hand_button_UP - hand_button_DOWN)
    this->servoAngle[3] = constrain(this->servoAngle[3] +da, hand_min, hand_max);
  }
  
  void updateServos(){
    //for(int i=0; i<4; i++){
      this->pwm.setPWM(Channels[0],0,map(180-constrain(this->servoAngle[0], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
      this->pwm.setPWM(Channels[1],0,map(180-constrain(this->servoAngle[1], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
      this->pwm.setPWM(Channels[2],0,map(180-constrain(this->servoAngle[2], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
      this->pwm.setPWM(Channels[3],0,map(constrain(this->servoAngle[3], 0, 180), 0, 180, SERVOMIN, SERVOMAX));
    //}
  }

};



