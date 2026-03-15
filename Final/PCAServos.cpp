#include "PCAServos.h"

IK2::IK2(const int jointLen[]): jointLength(jointLen){
  this->jointLength_sq[0] = sq(this->jointLength[0]);
  this->jointLength_sq[1] = sq(this->jointLength[1]);
}

void IK2::calcWrist(){
  float x = 0, y = 0, ang = 0;
  for(int i=0; i<2; i++){
    ang += i-PI;
    x += this->jointLength[i] *cos(this->jointAngle[i]);
    y += this->jointLength[i] *sin(this->jointAngle[i]);
    //this->positon[i][0] = x;
    //this->positon[i][1] = y;
  }
  this->setWrist(x, y);
}

bool IK2::setWrist(float x, float y){
  this->position[1][0] = x;
  this->position[1][1] = y;
  return this->isWristOK();
}

void IK2::calcAngle(){
  this->distance_sq = sq(this->position[1][0]) + sq(this->position[1][1]); this->distance = sqrt(this->distance_sq);
  this->jointAngle[0] = clip2pi(acos(constrain((
    (this->distance_sq +this->jointLength_sq[0] -this->jointLength_sq[1]) / (2 *this->distance *this->jointLength[0])
  ),-1.0f,1.0f)) + atan2(this->position[1][1], this->position[1][0]) );
  this->jointAngle[1] = clip2pi(acos(constrain((
    (this->jointLength_sq[0] +this->jointLength_sq[1] -this->distance_sq) / (2 *this->jointLength[0] *this->jointLength[1])
  ),-1.0f,1.0f)));

}

bool IK2::isWristOK(){
  return this->distance < this->jointLength[0] + this->jointLength[1] && this->distance > this->jointLength[0] - this->jointLength[1];
}

//////////////////////////////////////////

ServoMotors::ServoMotors(const bool usePCA, const int* armlens) :
  PCAEnable(usePCA), Ik(armlens) {}

void ServoMotors::begin(){
  if(this->PCAEnable){
    this->pwm.begin();
    this->pwm.setPWMFreq(50);
  }
  //this->moveWrist(arm_pos_init[0], arm_pos_init[1]);
  //this->rotate(SRV_WRIST, arm_pos_init[2]);
  this->setPosition(arm_pos_init);
}

void ServoMotors::setPosition(const float* command){
  this->Ik.setWrist(command[0], command[1]);
  this->angle[SRV_WRIST] = command[2];
  //this->Ik.calcAngle();
  //this->angle[SRV_SHOULDER] = degrees(this->Ik.jointAngle[SRV_SHOULDER]);
  //this->angle[SRV_ELBOW] = degrees(this->Ik.jointAngle[SRV_ELOBOW]);
}

void ServoMotors::rotate (int idx, int direc){
  if(idx < 0 || idx >= this->NUM) return;
  this->angle[idx] = constrain(this->angle[idx] +(direc*rotate_speed[idx]), limit_min[idx], limit_max[idx]);
  if(idx == SRV_SHOULDER || idx == SRV_ELBOW){
    this->Ik.jointAngle[idx] = radians(this->angle[idx]);
    this->Ik.calcWrist();
  }
  //switch(idx){
    //case SRV_SHOULDER:
    //case SRV_ELBOW:
      //this->Ik.jointAngle[idx] += radians(da);
      //this->Ik.calcWrist();
    //case SRV_WRIST:
    //case SRV_FINGER:
      //this->angle[idx] = constrain(this->angle[idx] +(direc*rotate_speed[idx]), limit_min[idx], limit_max[idx]);
}

void ServoMotors::moveWrist(int dx, int dy){
  if(!this->Ik.setWrist(this->Ik.position[1][0] +dx, this->Ik.position[1][1] +dy)){
    this->Ik.setWrist(this->Ik.position[1][0] -dx, this->Ik.position[1][1] -dy);
  }
  this->Ik.calcAngle();
  this->angle[SRV_SHOULDER] = degrees(this->Ik.jointAngle[SRV_SHOULDER]);
  this->angle[SRV_ELBOW] = degrees(this->Ik.jointAngle[SRV_ELBOW]);
}

void ServoMotors::updateAll(const bool* inversed){
  if(!this->PCAEnable) return;
  for(int idx=0; idx<NUM; idx++){
    if(inversed[idx]){
      pwm.setPWM(PCAChannels[idx], 0, map(180 -this->angle[idx] +InitalAngle[idx], 0,180,SERVOMIN,SERVOMAX));
    }else{
      pwm.setPWM(PCAChannels[idx], 0, map(this->angle[idx] +InitalAngle[idx],0,180,SERVOMIN,SERVOMAX));
    }
  }
}

