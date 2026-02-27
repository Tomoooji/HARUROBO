#include <PS4Controller.h>
#include <PCA9685.h>
//!!!!!実機でサーボの向きが逆の場合はmap関数の引数を入れ替えるだけで修正できる!!!!!
//アームの目的地点を0.8ずつ加算して更新しているが実際の使用感に合わせて変更する
// --- 定数・ピン定義 ---
#define SERVOMIN 120
#define SERVOMAX 470
PCA9685 pwm = PCA9685(0x40);

// 足回りピン (fr1, fr2, fl1, fl2, bl1, bl2, br1, br2)
//int pins[] = {15, 2, 0, 4, 16, 17, 18, 5};
//            1G  1Y  4G  4Y  3Y<>3G  2G 2Y 
int pins[] = {17, 16, 26, 27, 12, 14, 2, 15}; 

// --- アーム変数 ---
float target[3] = {15, 15, 0.75 * PI}; // IK: X(前後), Y(上下), 手首角度
float baseAngle = 90;                  // 1軸目: 旋回
float gripperPos = 90;                 // 5軸目: ハンド
float angles[3] = {0, 0, 0};           // IK算出角度
const float lengths[3] = {36, 26, 21};
const float sqlen[2] = {sq(lengths[0]), sq(lengths[1])};

void setup() {
  Serial.begin(115200);
  PS4.begin("e4:65:b8:d8:d4:80"); // MACアドレス書き換え済みです

  pwm.begin();//33,25);
  pwm.setPWMFreq(50);

  for(int i=0; i<8; i++) ledcAttach(pins[i], 12800, 8);
  Serial.println("Robot Controller Ready.");
}

void loop() {
  if (PS4.isConnected()) {
    // --- 1. 足回り制御 (左スティック移動 + R1/L1回転) ---
    int moveX = map(PS4.LStickX(), -128, 127, -100, 100);
    int moveY = map(PS4.LStickY(), -128, 127, -100, 100);
    int turn = 0;
    //!!!!!"turn=60"この60を変更して回転スピードを調整すること!!!!!
    if (PS4.R1()) turn = 60; // その場回転(時計回り)
    if (PS4.L1()) turn = -60; // その場回転(反時計回り)
    
    if (abs(moveX) < 15) moveX = 0;
    if (abs(moveY) < 15) moveY = 0;
    controlOmni(moveX, moveY, turn);

    //--- 2. アーム前後上下 (右スティック) ---
    int rx = PS4.RStickX(); // 前後(X)
    int ry = PS4.RStickY(); // 上下(Y)
    if (abs(rx) > 20) target[0] += (rx > 0 ? 0.8 : -0.8);
    if (abs(ry) > 20) target[1] += (ry > 0 ? 0.8 : -0.8);
    //！！！！！！！ 実際のアームで物理的に届かない位置にならないように要調整！！！！！！
    target[0] = constrain(target[0], 5, 50);
    target[1] = constrain(target[1], 0, 50);

    // --- 3. アーム旋回 (十字キー左右) ---
    if (PS4.Right()) baseAngle += 2.0;
    if (PS4.Left())  baseAngle -= 2.0;
    baseAngle = constrain(baseAngle, 0, 180);

    // --- 4. 手首角度 (◯/✕ボタン) ---
    if (PS4.Circle()) target[2] += 0.05;
    if (PS4.Cross())  target[2] -= 0.05;
    target[2] = clip2pi(target[2]);

    // --- 5. ハンド開閉 (L2/R2トリガー) ---
    if (PS4.R2Value() > 50) gripperPos += 3; // 閉じる
    if (PS4.L2Value() > 50) gripperPos -= 3; // 開く
    gripperPos = constrain(gripperPos, 40, 140);

    solveIK();
    updateServos();

  } else {
    stopAll();
  }
  delay(15);
}

// --- 逆運動学・サーボ出力 ---
void solveIK() {
  float wristX = (lengths[2] * cos(target[2])) + target[0];
  float wristY = (lengths[2] * sin(target[2])) + target[1];
  float dist2 = sq(wristX) + sq(wristY);
  float dist = sqrt(dist2);
  angles[0] = clip2pi(acos(constrain((dist2 + sqlen[0] - sqlen[1]) / (2 * dist * lengths[0]), -1.0f, 1.0f)) + atan2(wristY, wristX));
  angles[1] = clip2pi(acos(constrain((sqlen[0] + sqlen[1] - dist2) / (2 * lengths[0] * lengths[1]), -1.0f, 1.0f)));
  angles[2] = clip2pi(target[2] - angles[0] - angles[1] + (PI / 2));
}

void updateServos() {
  pwm.setPWM(0, 0, map(baseAngle, 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(2, 0, map(degrees(angles[0]), 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(4, 0, map(degrees(angles[1]), 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(6, 0, map(degrees(angles[2]), 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(8, 0, map(gripperPos, 0, 180, SERVOMIN, SERVOMAX));
}

// --- 足回り（回転対応版） ---
void controlOmni(int x, int y, int turn) {
  float r = sqrt(sq(x) + sq(y));
  float t = 0.25 * PI + atan2(y, x);
  int s1 = cos(t) * (r * 2.55);
  int s2 = sin(t) * (r * 2.55);

  // 各輪に旋回成分(turn)を加減算
  drivemotor(s1 + turn, pins[0], pins[1]); // fr
  drivemotor(s2 + turn, pins[2], pins[3]); // fl
  drivemotor(s1 - turn, pins[4], pins[5]); // bl
  drivemotor(s2 - turn, pins[6], pins[7]); // br
}

void drivemotor(int speed, int p1, int p2) {
  int pwm_val = constrain(abs(speed), 0, 255);
  if (speed > 10) { ledcWrite(p1, pwm_val); ledcWrite(p2, 0); }
  else if (speed < -10) { ledcWrite(p1, 0); ledcWrite(p2, pwm_val); }
  else { ledcWrite(p1, 0); ledcWrite(p2, 0); }
}

float clip2pi(float ang) { return ang < -PI ? 2*PI+ang : (ang > PI ? -2*PI+ang : ang); }
void stopAll() { for(int i=0; i<8; i++) ledcWrite(pins[i], 0); }