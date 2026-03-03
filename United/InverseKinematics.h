//#pragma once
#ifndef INVERSEKINEMATICS_H
#define INVERSEKINEMATICS_H
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
  IK3(const int lengths[], const float angles[]);
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

#define SERVOMIN 120
#define SERVOMAX 470
constexpr int Channels[] = {
  0, 2, 4, 6
};

inline PCA9685 servo_pwm = PCA9685(0x40);

inline void updateservo(IK3& ik, int& hand){//, int const channels[]){
  for(int i=0; i<3; i++){
    servo_pwm.setPWM(Channels[i], 0, map(constrain(degrees(clip2pi(PI-ik.servoAngle[i])),10,170),0,180,SERVOMIN, SERVOMAX));
  }
  servo_pwm.setPWM(Channels[3],0,map(constrain(hand,10,170),0,180,SERVOMIN, SERVOMAX));
}

#endif