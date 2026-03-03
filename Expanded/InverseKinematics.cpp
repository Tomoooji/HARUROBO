#include "InverseKinematics.h"

IK3::IK3(const int lengths[], const float angles[]) :
  armLength(lengths), defaultAngle(angles) {
  this->_sqLength[0] = sq(this->armLength[0]);
  this->_sqLength[1] = sq(this->armLength[1]);
}

void InitIK(IK3& ik){
  setTarget(ik, 10, 10);//
  setWrist(ik,0);
  calcAngle(ik);
}

void setTarget(IK3& ik, float x, float y){//, float a){
  moveTarget(ik, x-ik._wristx, y-ik._wristy);//, a-ik.a);
}

void moveTarget(IK3& ik, float dx, float dy){//, float da){
  if(dx || dy){// || da){
    ik._wristx += constrain(dx, -MAX_SPEED_XY, MAX_SPEED_XY);
    ik._wristy += constrain(dy, -MAX_SPEED_XY, MAX_SPEED_XY);
    //ik.a = clip2pi(ik.a + constrain(da, -MAX_SPEED_A, MAX_SPEED_A));
    //ik._wristx = ik.armLength[2] * cos(ik.a) + ik.x;
    //ik._wristy = ik.armLength[2] * sin(ik.a) + ik.y;
  }
}

void setWrist(IK3& ik, float a){
  ik.servoAngle[2] = a;
}
void moveWrist(IK3& ik, float da){
  moveWrist(ik, ik.jointAngle[2]+da);
}

void calcAngle(IK3& ik){
  ik._sqdist = sq(ik._wristx) + sq(ik._wristy);
  ik._dist = sqrt(ik._sqdist);
  // 解析的手法でservoAngle(ほんとはjointAngleにすべき)を計算
  ik.jointAngle[0] = clip2pi(acos(constrain(
      (ik._sqdist + ik._sqLength[0] - ik._sqLength[1]) / (2 * ik._dist * ik.armLength[0]),-1.0f,1.0f)) + atan2(ik._wristy,ik._wristx));
  ik.jointAngle[1] = clip2pi(acos(constrain(
      (ik._sqLength[0] + ik._sqLength[1] - ik._sqdist) / (2 * ik.armLength[0] * ik.armLength[1]),-1.0f,1.0f)));
  //ik.jointAngle[2] = clip2pi(-ik.servoAngle[0] -ik.servoAngle[1] -(PI-ik.a) +(PI*2));
  // 取付時の角度ずれを補正
  ik.servoAngle[0] = clip2pi(ik.jointAngle[0] - ik.defaultAngle[0]);
  ik.servoAngle[1] = clip2pi(ik.jointAngle[1] - ik.defaultAngle[1]);
  
  //ik.servoAngle[2] = clip2pi(ik.jointAngle[2] - ik.defaultAngle[2]);
}

void getTarget(IK3& ik){
  ik.wristx = (cos(ik.jointAngle[0])*ik.armLength[0]) + (cos(ik.jointAngle[0]+ik.jointAngle[1]-PI)*ik.armLength[1]);
  ik.wristy = (sin(ik.jointAngle[0])*ik.armLength[0]) + (sin(ik.jointAngle[0]+ik.jointAngle[1]-PI)*ik.armLength[1]);
}


float clip2pi(float ang){
  return ang<0 ? 2*PI+ang : (ang>2*PI ? -2*PI+ang: ang); 
}
