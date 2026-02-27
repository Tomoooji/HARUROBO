#include <PS4Controller.h>
#include <PCA9685.h>

// --- 定数・ピン定義 ---
#define SERVOMIN 120
#define SERVOMAX 470
PCA9685 pwm = PCA9685(0x40);

// 足回りピン (fr1, fr2, fl1, fl2, bl1, bl2, br1, br2)
int pins[] = {17, 16, 26, 27, 12, 14, 2, 15}; 

// 【変更箇所】十字キーで制御する特定モーターのピン
const int motorIn1 = 25;
const int motorIn2 = 33;

// --- アーム変数 ---
float target[3] = {15, 15, 0.75 * PI}; // IK: X(前後), Y(上下), 手首角度
float baseAngle = 90;                  // 1軸目: 旋回
float gripperPos = 90;                 // 5軸目: ハンド
float angles[3] = {0, 0, 0};           // IK算出角度
const float lengths[3] = {36, 26, 21};
const float sqlen[2] = {sq(lengths[0]), sq(lengths[1])};

void setup() {
  Serial.begin(115200);
  
  // PS4コントローラのMACアドレス
  PS4.begin("e4:65:b8:d8:d4:80"); 

  pwm.begin();
  pwm.setPWMFreq(50);

  // 足回り用ピンの出力設定 (LEDC)
  for(int i=0; i<8; i++) {
    ledcAttach(pins[i], 12800, 8);
  }
  
  // 【変更箇所】特定モーター用ピンの初期化
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  stopMotor();

  Serial.println("Robot Ready (Specific Motor: Pins 25, 33)");
}

void loop() {
  if (PS4.isConnected()) {
    
    // --- 1. 足回り制御 (左スティック移動 + R1/L1回転) ---
    int moveX = map(PS4.LStickX(), -128, 127, -100, 100);
    int moveY = map(PS4.LStickY(), -128, 127, -100, 100);
    int turn = 0;
    if (PS4.R1()) turn = 120; 
    if (PS4.L1()) turn = -120; 
    
    if (abs(moveX) < 10) moveX = 0;
    if (abs(moveY) < 10) moveY = 0;
    controlOmni(moveX, moveY, turn);

    // --- 2. アーム前後上下 (右スティック) ---
    int rx = PS4.RStickX(); 
    int ry = PS4.RStickY(); 
    if (abs(rx) > 20) target[0] += (rx > 0 ? 1.5: -1.5);
    if (abs(ry) > 20) target[1] += (ry > 0 ? 1.5 : -1.5);
    target[0] = constrain(target[0], 5, 50);
    target[1] = constrain(target[1], 0, 50);

    // --- 3. アーム旋回 (十字キー左右) ---
    if (PS4.Right()) baseAngle += 4.0;
    if (PS4.Left())  baseAngle -= 4.0;
    baseAngle = constrain(baseAngle, 0, 180);

    // --- 4. 十字キー上下：特定モーター制御 (ピン25, 33) ---
    if (PS4.Up()) {
      moveForward();
    }
    else if (PS4.Down()) {
      moveBackward();
    }
    else {
      stopMotor();
    }

    // --- 5. 手首角度 (◯/✕ボタン) ---
    if (PS4.Circle()) target[2] += 0.1;
    if (PS4.Cross())  target[2] -= 0.1;
    target[2] = clip2pi(target[2]);

    // --- 6. ハンド開閉 (L2/R2トリガー) ---
    if (PS4.R2Value() > 50) gripperPos += 6; 
    if (PS4.L2Value() > 50) gripperPos -= 6; 
    gripperPos = constrain(gripperPos, 40, 140);

    solveIK();
    updateServos();

  } else {
    stopAll();   // 足回り停止
    stopMotor(); // 特定モーター停止
  }
  delay(10);
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

// --- 特定モーター制御関数 ---
void moveForward() {
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
}

void moveBackward() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
}

void stopMotor() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
}

float clip2pi(float ang) { return ang < -PI ? 2*PI+ang : (ang > PI ? -2*PI+ang : ang); }
void stopAll() { for(int i=0; i<8; i++) ledcWrite(pins[i], 0); }