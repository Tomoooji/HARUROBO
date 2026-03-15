#pragma once
#include <Arduino.h>

//(extern) constexpr int DCpins[];
constexpr int DCpins[]={
//  FR      BR      BL      FL    yagura 
   2, 15, 12, 14, 26, 27, 18,  5, 25, 33
};

enum MOTORID{
  MTR_FRONTRIGHT, MTR_BACKRIGHT, MTR_BACKLEFT, MTR_FRONTLEFT, MTR_YAGURA
};

enum SPEED_OPTIONS{
  SPEED_SLOW = 85,
  SPEED_MIDD = 100,
  SPEED_FAST = 200,
  SPEED_TURN = 50,
  SPEED_YAGURA = 100
};
constexpr float leg_motor_gains[] = {1, 1, 1, 1}; // 足回りモーターの速度定数[最大のn倍]
extern constexpr int leg_accel = 5;

//inline int sign(auto x){return (x>0)-(x<0);}

class DCmotors{ // functional class for controlling DC motors
 public:
  int NUM = 5;
  int speed[5];
  DCmotors(){}

  void begin(){ // attach pins and set 0 speed
    for(int pin:DCpins){
      ledcAttach(pin, 12800, 8);
    }
    driveAll();
  }

  void quit(){ // turn off all motors
    for(int id=0; id<NUM; id++){
      this->speed[id] = 0;
    }
    this->driveAll();
  }

  void driveAll(){ // drive all motors by its power
    for(int idx=0; idx<5; idx++){
      ledcWrite(DCpins[idx*2], (this->speed[idx]>0? constrain(speed[idx], 0, 255): 0));
      ledcWrite(DCpins[idx*2+1], (this->speed[idx]<0? constrain(-speed[idx], 0, 255): 0));
    }
  }

  bool addAccel(int idx, int target){ // increase speed depends on accel gain
    this->speed[idx] = speed[idx]+leg_accel<target ? speed[idx]+leg_accel : (speed[idx]-leg_accel>target? speed[idx]-leg_accel :target);
    return this->speed[idx] == target;
  }
};

constexpr float range_othogonal = radians(30);
inline void calcOmuni_d(int* result, int valueX, int valueY, int turn, int speed_l, int speed_r){
  // only set 8direction +stop to leg motors (not drive here)
  float angle = atan2(valueY, valueX);
  int direcX = valueX?(cos(angle)>cos(range_othogonal)) - (cos(angle)<-cos(range_othogonal)):0;
  int direcY = (sin(angle)>sin(range_othogonal)) - (sin(angle)<-sin(range_othogonal));
  result[MTR_FRONTRIGHT] = (speed_l*sign(direcY -direcX)) -(speed_r*turn) *leg_motor_gains[MTR_FRONTRIGHT];
  result[MTR_BACKRIGHT]  = (speed_l*sign(direcY +direcX)) -(speed_r*turn) *leg_motor_gains[MTR_BACKRIGHT];
  result[MTR_BACKLEFT]   = (speed_l*sign(direcY -direcX)) +(speed_r*turn) *leg_motor_gains[MTR_BACKLEFT];
  result[MTR_FRONTLEFT]  = (speed_l*sign(direcY +direcX)) +(speed_r*turn) *leg_motor_gains[MTR_FRONTLEFT];
}

inline void calcOmuni_accel_d(DCmotors& DC, int valueX, int valueY, int turn, int speed_l, int speed_r){
  float angle = atan2(valueY, valueX);
  int direcX = valueX?(cos(angle)>cos(range_othogonal)) - (cos(angle)<-cos(range_othogonal)):0;
  int direcY = (sin(angle)>sin(range_othogonal)) - (sin(angle)<-sin(range_othogonal));
  DC.addAccel(MTR_FRONTRIGHT,(speed_l*sign(direcY -direcX)) -(speed_r*turn) *leg_motor_gains[MTR_FRONTRIGHT]);
  DC.addAccel(MTR_BACKRIGHT ,(speed_l*sign(direcY +direcX)) -(speed_r*turn) *leg_motor_gains[MTR_BACKRIGHT]);
  DC.addAccel(MTR_BACKLEFT  ,(speed_l*sign(direcY -direcX)) +(speed_r*turn) *leg_motor_gains[MTR_BACKLEFT]);
  DC.addAccel(MTR_FRONTLEFT ,(speed_l*sign(direcY +direcX)) +(speed_r*turn) *leg_motor_gains[MTR_FRONTLEFT]);
  /*
  Serial.print(DC.speed[MTR_FRONTRIGHT]); Serial.print(",");
  Serial.print(DC.speed[MTR_BACKRIGHT]); Serial.print(",");
  Serial.print(DC.speed[MTR_BACKLEFT]); Serial.print(",");
  Serial.print(DC.speed[MTR_FRONTLEFT]); Serial.println();//*/
}

inline void calcOmuni_a(int* result, int valueX, int valueY, int turn, int speed_l, int speed_r){
  // set all angles from input to leg motors (not drive here)
  float angle = atan2(valueY, valueX);
  result[MTR_FRONTRIGHT]= (speed_l*(valueX?cos(angle+(PI/4)):0)) -(speed_r*turn) *leg_motor_gains[MTR_FRONTRIGHT];
  result[MTR_BACKRIGHT] = (speed_l*(valueY?sin(angle+(PI/4)):0)) -(speed_r*turn) *leg_motor_gains[MTR_BACKRIGHT];
  result[MTR_BACKLEFT]  = (speed_l*(valueX?cos(angle+(PI/4)):0)) +(speed_r*turn) *leg_motor_gains[MTR_BACKLEFT];
  result[MTR_FRONTLEFT] = (speed_l*(valueY?sin(angle+(PI/4)):0)) +(speed_r*turn) *leg_motor_gains[MTR_FRONTLEFT];
}

inline void calcOmuni_accel_a(DCmotors& DC, int valueX, int valueY, int turn, int speed_l, int speed_r){
  // set all angles from input to leg motors (not drive here) +accel
  float angle = atan2(valueY, valueX);
  DC.addAccel(MTR_FRONTRIGHT,(speed_l*(valueX?cos(angle+(PI/4)):0)) -(speed_r*turn) *leg_motor_gains[MTR_FRONTRIGHT]);
  DC.addAccel(MTR_BACKRIGHT, (speed_l*(valueY?sin(angle+(PI/4)):0)) -(speed_r*turn) *leg_motor_gains[MTR_BACKRIGHT]);
  DC.addAccel(MTR_BACKLEFT , (speed_l*(valueX?cos(angle+(PI/4)):0)) +(speed_r*turn) *leg_motor_gains[MTR_BACKLEFT]);
  DC.addAccel(MTR_FRONTLEFT, (speed_l*(valueY?sin(angle+(PI/4)):0)) +(speed_r*turn) *leg_motor_gains[MTR_FRONTLEFT]);
}
