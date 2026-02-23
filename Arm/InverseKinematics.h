#ifndef INVERSE_KINEMATICS_H
#define INVERSE_KINEMATICS_H

#include <Arduino.h>

constexpr int MAX_SPEED_XY = 10;
constexpr float MAX_SPEED_A = 0.5;

struct IK3{
  const int* armLength;
  const float* defaultAngle;
  int x; //floatなんてつかわんやろ
  int y;
  float a; //radians
  float jointAngle[3];// ほんとはこっちで計算した方が良い
  float servoAngle[3];// ただもとの式がこっち準拠なのでこっちで計算
  int _sqLength[2];
  float _wristx;
  float _wristy;
  float _dist;
  float _sqdist;
  float _aMinus;
  IK3(const int* lengths, const float* angles);
};

void InitIK(IK3& ik);
void setTarget(IK3& ik, int x, int y, float a);
void moveTarget(IK3& ik, int dx, int dy, float da);
void calcAngle(IK3& ik);
float clip2pi(float ang);

#endif