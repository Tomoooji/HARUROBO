#include <PS4Controller.h>
#include <PCA9685.h>

// --- 定数・ピン設定 ---
#define SERVOMIN 120 
#define SERVOMAX 470 
PCA9685 pwm = PCA9685(0x40); 

// 足回りモーターピン (右前, 右後, 左後, 左前の順)
int pins[] = {16, 17, 2, 15, 14, 12, 26, 27}; 

// 補助モーター（やぐら運搬用）
const int motorIn1 = 25;
const int motorIn2 = 33;
int motorSpeed = 200; 

// --- アーム物理パラメータ ---
const float lengths[3] = {36.0, 26.0, 21.0}; // リンク長(cm)
const float sqlen[2] = {sq(lengths[0]), sq(lengths[1])}; 

// --- 操作状態変数 ---
float target[3] = {0.0, 0.0, 0.0}; // X, Y, 手首角度(rad)
float angles[3] = {0.0, 0.0, 0.0};   // 計算後のサーボ角度
const int initang[3] = {0,0,90}; 
float gripperPos = 90.0;             // ハンド開閉

void setup() {
  Serial.begin(9600);
  PS4.begin("e4:65:b8:d8:d4:80"); // お使いのMACアドレス
  pwm.begin();
  pwm.setPWMFreq(50); 
  
  // 足回りモーター & 補助モーターをPWM設定
  for(int i=0; i<8; i++) ledcAttach(pins[i], 12800, 8);
  ledcAttach(motorIn1, 12800, 8);
  ledcAttach(motorIn2, 12800, 8);
  
  stopAll();
}

void loop() {
  if (PS4.isConnected()) {
    // 1. 足回り制御（8方向スナップ機能付き）
    handleChassis();

    // 2. アーム座標制御（右スティック）
      //handleArmCoordinates();
    int rx = PS4.RStickX(); //
    int ry = PS4.RStickY(); ///
    if (abs(rx) > 20) target[0] += (rx > 0 ? 1: -1);
    if (abs(ry) > 20) target[1] += (ry > 0 ? 1 :-1);
    target[0] = constrain(target[0], -10, 50);
    target[1] = constrain(target[1], -10, 50);

    // 3. 補助モーター制御（十字キー左右：やぐら運搬）
    if (PS4.Right()) moveForward();
    else if (PS4.Left()) moveBackward();
    else stopMotor();

    // 4. 手首・ハンド制御
    if (PS4.Circle()) target[2] += 0.08; 
    if (PS4.Cross())  target[2] -= 0.08; 
    //target[2] = constrain(target[2], -HALF_PI, HALF_PI);
    target[2] = clip2pi(target[2]);

    if (PS4.R2Value() > 50) gripperPos += 6; 
    if (PS4.L2Value() > 50) gripperPos -= 6; 
    gripperPos = constrain(gripperPos, 40, 140);

    // 5. 反映
    solveIK();
    updateServos();
  } else {
    stopAll();
  }
  delay(10);
}

// --- 足回り制御関数 ---
void handleChassis() {
  int rawX = PS4.LStickX();
  int rawY = PS4.LStickY();
  int moveX = 0, moveY = 0;
  float magnitude = sqrt(sq(rawX) + sq(rawY));
  
  if (magnitude > 40) {
    float angle = atan2(rawY, rawX) * 180.0 / PI;
    if (angle < 0) angle += 360;
    
    float cardinalWidth = 20.0; // 上下左右を判定する遊び幅
    
    // スナップ処理：純粋な前後左右よりも「斜め」を出しやすく調整
    if (angle > (360-cardinalWidth) || angle < cardinalWidth) { moveX = 100; moveY = 0; }
    else if (angle > (90-cardinalWidth) && angle < (90+cardinalWidth)) { moveX = 0; moveY = 100; }
    else if (angle > (180-cardinalWidth) && angle < (180+cardinalWidth)) { moveX = -100; moveY = 0; }
    else if (angle > (270-cardinalWidth) && angle < (270+cardinalWidth)) { moveX = 0; moveY = -100; }
    else {
      if (angle > 0 && angle < 90) { moveX = 100; moveY = 100; }
      else if (angle > 90 && angle < 180) { moveX = -100; moveY = 100; }
      else if (angle > 180 && angle < 270) { moveX = -100; moveY = -100; }
      else { moveX = 100; moveY = -100; }
    }
  }
  
  int turn = 0;
  if (PS4.R1()) turn = 120;
  if (PS4.L1()) turn = -120;
  controlOmni(moveX, moveY, turn);
}

// オムニホイール運動学計算 (新インデックス順: 右前, 右後, 左後, 左前)
void controlOmni(int x, int y, int t) {
  driveMotor(0, y - x - t); // 右前
  driveMotor(1, y + x - t); // 右後
  driveMotor(2, y - x + t); // 左後
  driveMotor(3, y + x + t); // 左前
}

void driveMotor(int index, int speed) {
  speed = constrain(speed, -255, 255);
  int in1 = index * 2;
  int in2 = index * 2 + 1;
  if (speed > 0) {
    ledcWrite(pins[in1], speed);
    ledcWrite(pins[in2], 0);
  } else {
    ledcWrite(pins[in1], 0);
    ledcWrite(pins[in2], -speed);
  }
}

// --- アーム座標・IK制御 ---
void solveIK() {
  float wristX = (lengths[2] * cos(target[2])) + target[0];
  float wristY = (lengths[2] * sin(target[2])) + target[1];
  float dist2 = sq(wristX) + sq(wristY);
  float dist = sqrt(dist2);
  angles[0] = clip2pi(acos(constrain((dist2 + sqlen[0] - sqlen[1]) / (2 * dist * lengths[0]), -1.0f, 1.0f)) + atan2(wristY, wristX));
  angles[1] = clip2pi(acos(constrain((sqlen[0] + sqlen[1] - dist2) / (2 * lengths[0] * lengths[1]), -1.0f, 1.0f)));
  angles[2] = clip2pi(target[2] - angles[0] - angles[1] + (PI / 2));
  angles[2] = clip2pi((2*PI)-angles[0]-angles[1]-(PI-target[2]));
}

void updateServos() {
  pwm.setPWM(0, 0, map(constrain(degrees(angles[0])+initang[0],10,170), 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(2, 0, map(constrain(degrees(angles[1])+initang[1],10,170), 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(4, 0, map(constrain(degrees(angles[2])-initang[2],10,170), 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(6, 0, map(gripperPos, 0, 180, SERVOMIN, SERVOMAX));
  Serial.print(angles[0]);
  Serial.print(",");
  Serial.print(angles[1]);
  Serial.print(",");
  Serial.print(angles[2]);
  Serial.println();
}


/*void handleArmCoordinates() {
  int rx = PS4.RStickX();
  int ry = PS4.RStickY();
  if (abs(rx) > 20) target[0] += (rx > 0 ? 1 : -1);
  if (abs(ry) > 20) target[1] += (ry > 0 ? 1 : -1);
  target[0] = constrain(target[0], -10, 60);
  target[1] = constrain(target[1], -30, 60);
}

void solveIK() {
  // 手首の位置(wx, wy)を計算
  float wx = lengths[2] * cos(target[2]) + target[0];
  float wy = lengths[2] * sin(target[2]) + target[1];
  
  float d2 = sq(wx) + sq(wy);
  float d = sqrt(d2);
  
  // 余弦定理で各関節角を算出
  //float a = acos((sqlen[0] + sqlen[1] - d2) / (2 * lengths[0] * lengths[1]));
  angles[1] = clip2pi(-acos((sqlen[0] + sqlen[1] - d2) / (2 * lengths[0] * lengths[1])));
  //angles[1] = PI - a; // 肘の角度
  
  float alpha = atan2(wy, wx);
  float beta = acos((sqlen[0] + d2 - sqlen[1]) / (2 * lengths[0] * d));
  angles[0] = clip2pi(-(alpha + beta)); // 肩の角度
  
  angles[2] = clip2pi(-(target[2] - angles[0] - angles[1] + HALF_PI)); // 手首の相対角度
}

void updateServos() {
  // PCA9685へ角度（パルス幅）を送信
  for (int i = 0; i < 3; i++) {
    int pulse = map(constrain(degrees(angles[i]),0,180), 0, 180, SERVOMIN, SERVOMAX);
    pwm.setPWM(i*2, 0, pulse);
    Serial.print(degrees(angles[i]));
    if(i<2){
      Serial.print(",");
    }else{
      Serial.println();
    }
  }
  pwm.setPWM(6, 0, map(gripperPos, 0, 180, SERVOMIN, SERVOMAX));
}
*/
// --- 補助モーター・安全停止 ---
void moveForward() { ledcWrite(motorIn1, motorSpeed); ledcWrite(motorIn2, 0); }
void moveBackward() { ledcWrite(motorIn1, 0); ledcWrite(motorIn2, motorSpeed); }
void stopMotor() { ledcWrite(motorIn1, 0); ledcWrite(motorIn2, 0); }

void stopAll() {
  for (int i = 0; i < 4; i++) driveMotor(i, 0);
  stopMotor();
}

float clip2pi(float ang) { return ang < -PI ? 2*PI+ang : (ang > PI ? -2*PI+ang : ang); }

/*float clip2pi(float ang){
  return ang<0 ? 2*PI+ang : (ang>2*PI ? -2*PI+ang: ang); 
}
*/