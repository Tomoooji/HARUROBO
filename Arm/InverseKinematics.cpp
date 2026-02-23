#include "InverseKinematics.h"

IK3::IK3(const int* lengths, const float* angles) :
  armLength(lengths), defaultAngle(angles) {
  this->_sqLength[0] = sq(this->armLength[0]);
  this->_sqLength[1] = sq(this->armLength[1]);
}

void InitIK(IK3& ik){
  setTarget(ik, 10, 10, 0.75*PI);
  calcAngle(ik);
}

void setTarget(IK3& ik, int x, int y, float a){
  moveTarget(ik, x-ik.x, y-ik.y, a-ik.a);
}

void moveTarget(IK3& ik, int dx, int dy, float da){
  if(dx || dy || da){
    ik.x += constrain(dx, -MAX_SPEED_XY, MAX_SPEED_XY);
    ik.y += constrain(dy, -MAX_SPEED_XY, MAX_SPEED_XY);
    ik.a = clip2pi(ik.a + constrain(da, -MAX_SPEED_A, MAX_SPEED_A));
    ik._wristx = ik.armLength[2] * cos(ik.a) + ik.x;
    ik._wristy = ik.armLength[2] * sin(ik.a) + ik.y;
  }
}

void calcAngle(IK3& ik){
  ik._sqdist = sq(ik._wristx) + sq(ik._wristy);
  ik._dist = sqrt(ik._sqdist);
  // 解析的手法でservoAngle(ほんとはjointAngleにすべき)を計算
  ik.servoAngle[0] = clip2pi(acos(constrain(
      (ik._sqdist+ik._sqLength[0]-ik._sqLength[1])/(2*ik._dist*ik.armLength[0]),-1.0f,1.0f)) + atan2(ik._wristy,ik._wristx));
  ik.servoAngle[1] = clip2pi(acos(constrain(
      (ik._sqLength[0]+ik._sqLength[1]-ik._sqdist)/(2*ik.armLength[0]*ik.armLength[1]),-1.0f,1.0f)));
  ik.servoAngle[2] = clip2pi(ik.a - ik.servoAngle[0] - ik.servoAngle[1] + (PI/2));
  // 取付時の角度ずれを補正
  ik.servoAngle[0] = clip2pi(ik.servoAngle[0] - ik.defaultAngle[0]);
  ik.servoAngle[1] = clip2pi(ik.servoAngle[1] - ik.defaultAngle[1]);
  ik.servoAngle[2] = clip2pi(ik.servoAngle[2] - ik.defaultAngle[2]);
}

float clip2pi(float ang){
  return ang<0 ? 2*PI+ang : (ang>2*PI ? -2*PI+ang: ang); 
}
