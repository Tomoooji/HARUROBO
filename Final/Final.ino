//--入力用変数--//
bool connection_flag;
bool disconnect_button;
int leg_joystick_x, leg_joystick_y;
//int leg_direc;
bool leg_button_R, leg_button_L;
bool leg_button_shift;///////
bool yagura_R, yagura_L;
int arm_joystick_x, arm_joystick_y;
//bool arm_zplus, arm_zminus, arm_yplus, arm_yminus;
bool arm_button_UP, arm_button_DOWN;
bool arm_button_init, arm_button_pick, arm_button_drop;
bool finger_button_UP, finger_button_DOWN;
bool clow_button_UP, clow_button_DOWN;
bool appeal_button;

bool shoulder_button_UP, shoulder_button_DOWN;
bool elbow_button_UP, elbow_button_DOWN;

//PS4_CONTROLLER or REMOTEXY_BTCL or REMOTEXY_BLE or SERIAL_CONTROLLER
#define PS4_CONTROLLER
#include "SwitchMode.h"

//--出力用定数--//
constexpr int led_power = 150;


//--環境依存定数--//
constexpr int LEDpins[]={32, 4, 0};

//--出力用変数--//
#include "DCmotors.h"
#include "PCAServos.h"

//Arm sakuarm(ArmLength, InitalAngle);
ServoMotors Arm = ServoMotors(false);
DCmotors DCs;

void setup(){
  Serial.begin(9600);

  //--init controller--//
  PS4.begin(MAC_PS4CON);
  //RemoteXY_Init();
  ////Serial2.begin(115200,SERIAL_8N1, 16, 17);

  //--init DCmotors--//
  DCs.begin();
  //--init Arm--//
  Arm.begin();

  //--init LED--//
  for(int pin: LEDpins){
    ledcAttach(pin,12800,8);
    ledcWrite(pin,40);
  }
}

void loop(){
  //--get inputs--//
  PS4Input();
  //RemoteXYEngine.handler(); RemoteXYInput();
  //connection_flag = SerialInput();

  //--process logic--//
  //if(disconnect_button && connection_flag) connection_flag = !connection_flag;
  if(appeal_button){
      for(int pin: DCpins){
      ledcWrite(pin,0);
    }
    for(int pin:LEDpins){
      ledcWrite(pin, led_power);
    }
    delay(400);
    //RemoteXYEngine.delay(400);
    for(int pin:LEDpins){
      ledcWrite(pin, 0);
    }
    delay(400);
    //RemoteXYEngine.delay(400);
  }
  else if(connection_flag){

  //-manage omuni-//
    if(isStickmoved(leg_joystick_x, leg_joystick_y, range_ignoreLstick)){
      calcOmuni_accel_d(DCs, leg_joystick_x , leg_joystick_y, (leg_button_R - leg_button_L),
                        (leg_button_shift?SPEED_FAST:SPEED_SLOW), SPEED_TURN);
    }else{
      calcOmuni_accel_d(DCs, 0, 0, (leg_button_R - leg_button_L), SPEED_FAST, SPEED_TURN);
    }
    DCs.speed[MTR_YAGURA] = SPEED_YAGURA * (yagura_R - yagura_L);
    DCs.driveAll();
  //*/

  //-manage ikarm-//
    if(arm_button_init){
      Arm.setPosition(arm_pos_init);
    }
    if(arm_button_pick){
      Arm.setPosition(arm_pos_pick);
    }
    if(arm_button_drop){
      Arm.setPosition(arm_pos_drop);
    }
    if(isStickmoved(arm_joystick_x, arm_joystick_y, range_ignoreRstick)){
      Arm.moveWrist(arm_speed *arm_joystick_x, arm_speed *arm_joystick_y);    
    }

    Arm.rotate(SRV_SHOULDER, shoulder_button_UP - shoulder_button_DOWN);
    Arm.rotate(SRV_ELBOW, elbow_button_UP - elbow_button_DOWN);
    Arm.rotate(SRV_WRIST, arm_button_UP - arm_button_DOWN);
    Arm.rotate(SRV_FINGER, finger_button_UP - finger_button_DOWN);

    Arm.updateAll();
  //*/

  }else{
    for(int pin: DCpins){
      ledcWrite(pin,0);
    }
    DCs.quit();
  }
  //*------------------
  Serial.print(Arm.angle[0]); Serial.print(",");
  Serial.print(Arm.angle[1]); Serial.print(",");
  Serial.print(Arm.angle[2]); Serial.print(",");
  Serial.print(Arm.angle[3]); Serial.print(",");
  Serial.print(Arm.Ik.position[1][0]); Serial.print(",");
  Serial.print(Arm.Ik.position[1][1]); Serial.println();
  //*///-----------------
  ledcWrite(LEDpins[0],connection_flag*led_power);

  delay(10);
  //RemoteXYEngine.delay(10);
}


