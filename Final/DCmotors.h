//constexpr int DC_default_speed = 150; //             櫓用アームの回転速度(足回りは255)[n/255]
//constexpr int leg_speed_high = 200; //             櫓用アームの回転速度(足回りは255)[n/255]
//constexpr int leg_speed_low = 85; //             櫓用アームの回転速度(足回りは255)[n/255]
enum SPEED_OPTIONS{
  SLOW = 85,
  MIDDLE = 100,
  HIGH = 200,
  YAGURA = 100
};
constexpr float leg_motor_gains[] = {1, 1, 1, 1}; // 足回りモーターの速度定数[最大のn倍]
extern constexpr int leg_accel = 5;



enum MOTORID{
  FRONTRIGHT, //0
  BACKRIGHT,  //1
  BACKLEFT,   //2
  FRONTLEFT,  //3
  YAGURAARM   //4
};

struct DCmotor{
 public:
  const int NUM;
  int leg_direc1, leg_direc2;
  int speed[NUM];

  void begin(){
    for(int pin:DCpins){
      ledcAttach(pin);
    }
    driveAll();
  }

  void driveAll(){
    for(int idx=0; idx<5; idx++){
      ledcWrite(DCpins[idx*2], (this->speed[idx]>0? constrain(speed[idx], 0, 255): 0));
      ledcWrite(DCpins[idx*2+1], (this->speed[idx]<0? constrain(-speed[idx], 0, 255): 0));
    }
  }

  bool addAccel(int idx, int target){
    speed[idx] = target-speed[idx]>leg_accel? speed[idx] + leg_accel: target;
    return speed[idx] == target;
  }
};

inline void setDirection()

inline void calcOmuni(DCmotor& leg, int direcX, int direcY, int turn, int speed_l, int speed_r, bool useaccel = false){
  if(!useaccel){
    leg.speed[DCmotor::FRONTRIGHT] = (speed_l*sign(direcY -direcX)) - (speed_r*turn) *leg_motor_gains[DCmotor::FRONTRIGHT]
    leg.speed[DCmotor::BACKRIGHT]  = (speed_l*sign(direcY +direcX)) - (speed_r*turn) *leg_motor_gains[DCmotor::BACKRIGHT]
    leg.speed[DCmotor::BACKLEFT]   = (speed_l*sign(direcY -direcX)) + (speed_r*turn) *leg_motor_gains[DCmotor::BACKLEFT]
    leg.speed[DCmotor::FRONTLEFT]  = (speed_l*sign(direcY +direcX)) + (speed_r*turn) *leg_motor_gains[DCmotor::FRONTLEFT]
  }
  else{
    addAccel(leg.speed[DCmotor::FRONTRIGHT],(speed_l*sign(direcY -direcX)) - (speed_r*turn) *leg_motor_gains[DCmotor::FRONTRIGHT]);
    addAccel(leg.speed[DCmotor::BACKRIGHT], (speed_l*sign(direcY +direcX)) - (speed_r*turn) *leg_motor_gains[DCmotor::BACKRIGHT]);
    addAccel(leg.speed[DCmotor::BACKLEFT] , (speed_l*sign(direcY -direcX)) + (speed_r*turn) *leg_motor_gains[DCmotor::BACKLEFT]);
    addAccel(leg.speed[DCmotor::FRONTLEFT], (speed_l*sign(direcY +direcX)) + (speed_r*turn) *leg_motor_gains[DCmotor::FRONTLEFT]);
  }
}
