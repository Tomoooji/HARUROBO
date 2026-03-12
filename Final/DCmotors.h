extern constexpr int DCpins[];
extern constexpr float leg_motor_gains[]
extern constexpr int leg_accel;
extern enum SPEED_OPTIONS{
  SLOW = 85,
  MIDDLE = 100,
  HIGH = 200,
  YAGURA = 100
};


struct DCmotor{
 public:
  //*
  enum MOTORID{
  FRONTRIGHT, //0
  BACKRIGHT,  //1
  BACKLEFT,   //2
  FRONTLEFT,  //3
  YAGURAARM   //4
  };
  //*/
  int leg_direc1, leg_direc2;
  int current_speed[5] = {0,0,0,0,0};

  void begin(){
    for(int pin:DCpins){
      ledcAttach(pin);
    }
    driveAll();
  }

  void driveAll(){
    for(int idx=0; idx<5; idx++){
      ledcWrite(DCpins[idx*2], (this->current_speed[idx]>0? current_speed[idx]: 0));
      ledcWrite(DCpins[idx*2+1], (this->current_speed[idx]<0? -current_speed[idx]: 0));
    }
  }

  bool addAccel(int idx, int target){
    current_speed[idx] = target-current_speed[idx]>leg_accel? current_speed[idx] + leg_accel: target;
    return current_speed[idx] == target;
  }
};

inline void calcOmuni(DCmotor& leg, int direcX, int direcY, int turn, int speedVal){
  leg.current_speed[DCmotor::FRONTRIGHT] = speedVal*sign(direcY -direcX -turn)*leg_motor_gains[DCmotor::FRONTRIGHT]
  leg.current_speed[DCmotor::BACKRIGHT] = speedVal*sign(direcY +direcX -turn)*leg_motor_gains[DCmotor::BACKRIGHT]
  leg.current_speed[DCmotor::BACKLEFT] = speedVal*sign(direcY -direcX +turn)*leg_motor_gains[DCmotor::BACKLEFT]
  leg.current_speed[DCmotor::FRONTLEFT] = speedVal*sign(direcY +direcX +turn)*leg_motor_gains[DCmotor::FRONTLEFT]
  //addAccel(leg.current_speed[leg::FRONTRIGHT], speedVal*sign(direcY -direcX -turn)*leg_motor_gains[leg::FRONTRIGHT])
}
