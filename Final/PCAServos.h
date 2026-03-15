#pragma once
#include <PCA9685.h>
constexpr int SERVOMAX = 470, SERVOMIN = 120;
#include <Arduino.h>

//(extern) constexpr int PCAChannels[];
constexpr int PCAChannels[] = {
  0, 2, 4, 6, 8, 10
};


enum SERVOID{
  SRV_SHOULDER, SRV_ELBOW, SRV_WRIST, SRV_FINGER
};

constexpr int ArmLength[] = {36, 26, 10};
constexpr int InitalAngle[] = {0, 0, 90};
constexpr bool SetInversed[] = {true, true, true, false};

constexpr int limit_max[] = {180,180,180,140};
constexpr int limit_min[] = {0,0,0,40};
constexpr int rotate_speed[] = {3,3,3,6};
constexpr float arm_speed = 0.01;

constexpr float arm_pos_init[] = {10,  10, PI};
constexpr float arm_pos_pick[] = {162.24/4, -89.44/4, radians(135)};
constexpr float arm_pos_drop[] = {147.24/4, 152.36/4, 0.5};// 45.08/4, 80.88/4

inline float clip2pi(float ang){
  return (ang<0? ang+TWO_PI: (ang>TWO_PI? ang-TWO_PI: ang));
}

struct IK2{ // inner(not nested) class for inverse & forward kinematics
 public:
  const int* jointLength;
  int jointLength_sq[2];
  float position[2][2];
  float jointAngle[2];
  float distance_sq;
  float distance;
  IK2(const int jointLen[]);
  void calcWrist();
  bool setWrist(float x, float y);
  void calcAngle();
  bool isWristOK();
};

class ServoMotors{ // functional class for cotrolling servo motors with PCA9685
 private:
 public:
  int NUM = 4;
  const bool PCAEnable;
  IK2 Ik;
  PCA9685 pwm = PCA9685(0x40);
  int angle[4];
  ServoMotors(const bool usePCA = true, const int* armlens = ArmLength);
  void begin();
  void setPosition(const float* command);
  void rotate(int idx, int da);
  void moveWrist(int dx, int dy);
  void updateAll(const bool* inversed = SetInversed);
};

