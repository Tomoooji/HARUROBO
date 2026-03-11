#pragma once // これは気にしない
#include <Arduino.h>


constexpr int FRONTRIGHT = 0;
constexpr int BACKRIGHT  = 1;
constexpr int BACKLEFT   = 2;
constexpr int FRONTLEFT  = 3;
constexpr int YAGURAARM  = 4;


//-- LOGICs --//
// set 8 direction form -pi~pi angle
inline void setdirection(float inputAngle, int& direcX, int& direcY){
  // form -PI to PI
  if(inputAngle > PI -range_othogonal || inputAngle < -PI +range_othogonal){
    // LEFT
    direcX = -1; direcY = 0;
    Serial.println("L");
  }
  else if(inputAngle <= -0.5*PI -range_othogonal){
    // BACKLEFT
    direcX =-1; direcY =-1;
    Serial.println("BL");
  }
  else if(inputAngle < -0.5*PI +range_othogonal){
    // BACK
    direcX = 0; direcY =-1;
    Serial.println("B");
  }
  else if(inputAngle <= 0 -range_othogonal){
    // BACKRIGHT
    direcX = 1; direcY =-1;
    Serial.println("BR");
  }
  else if(inputAngle < 0 +range_othogonal){
    // RIGHT
    direcX = 1; direcY = 0;
    Serial.println("R");
  }
  else if(inputAngle <= 0.5*PI -range_othogonal){
    // FRONTRIGHT
    direcX = 1; direcY = 1;
    Serial.println("FR");
  }
  else if(inputAngle < 0.5*PI +range_othogonal){
    // FRONT
    direcX = 0; direcY = 1;
    Serial.println("F");
  }
  else if(inputAngle <= PI -range_othogonal){
    // FRONTLEFT
    direcX =-1; direcY = 1;
    Serial.println("FL");
  }
  else{
    direcX = 0; direcY = 0;
    Serial.println("N");
  }
}

//-- OUTPUTs --//

// drive dc motor
inline void drivemotor(int index, int speed = DC_default_speed){
    speed = constrain(speed, -255, 255);
  if (speed > 0) {
    ledcWrite(DCpins[index*2], speed);
    ledcWrite(DCpins[index*2+1], 0);
  } else {
    ledcWrite(DCpins[index*2], 0);
    ledcWrite(DCpins[index*2+1], -speed);
  }
}

// drive leg motors
inline void driveomuni(int direcX, int direcY, int turn, int speedVal = DC_default_speed){
  drivemotor(FRONTRIGHT,speedVal*sign(direcY -direcX -turn)*leg_motor_gains[FRONTRIGHT]);
  drivemotor(BACKRIGHT, speedVal*sign(direcY +direcX -turn)*leg_motor_gains[BACKRIGHT]);
  drivemotor(BACKLEFT , speedVal*sign(direcY -direcX +turn)*leg_motor_gains[BACKLEFT]);
  drivemotor(FRONTLEFT, speedVal*sign(direcY +direcX +turn)*leg_motor_gains[FRONTLEFT]);
}
