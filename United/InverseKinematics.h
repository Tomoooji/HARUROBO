#pragma once
#include <Arduino.h>

constexpr int MAX_SPEED_XY = 10;
constexpr float MAX_SPEED_A = 0.5;

struct IK3{
  const int* armLength;
  const float* defaultAngle;
  //float x;
  //float y;
  //float a; //radians
  float jointAngle[3];
  float servoAngle[3];
  int _sqLength[2];
  float _wristx;
  float _wristy;
  float _dist;
  float _sqdist;
  IK3(const int* lengths, const float* angles);
};

void InitIK(IK3& ik);
void setTarget(IK3& ik, float x, float y);//, float a);
void moveTarget(IK3& ik, float dx, float dy);//, float da);
void setWrist(IK3& ik, float a);
void moveWrist(IK3& ik, float da);
//void getTarget(IK& ik);
//void animate(IK3& ik);
void calcAngle(IK3& ik);
float clip2pi(float ang);

///////////////////////////////////////////////////////

#include <PCA9685.h>
PCA9685 pwm;
void updateservo(IK3& ik, int& hand){
  for(int i=0; i<3; i++){

  }

}

