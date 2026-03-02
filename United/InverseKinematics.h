#pragma once
#include <Arduino.h>

constexpr int MAX_SPEED_XY = 10;
constexpr float MAX_SPEED_A = 0.5;

struct IK3{
  const int* armLength;
  const float* defaultAngle;
  float X;
  float Y;
  float Ang; //radians
  float jointAngle[3];
  float servoAngle[3];
  int _sqLength[2];
  float _wristX;
  float _wristY;
  float _dist;
  float _sqdist;
  IK3(const int* lengths, const float* angles);
};

void InitIK(IK3& ik);
void setTarget(IK3& ik, float x, float y, float a);
void moveTarget(IK3& ik, float dx, float dy, float da);
void calcAngle(IK3& ik);
float clip2pi(float ang);
