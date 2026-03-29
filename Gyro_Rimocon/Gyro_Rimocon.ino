#include <ESPNOW_Rimocon.h>

constexpr uint8_t receiverAddress[] = {0x80, 0xf3, 0xda, 0x41, 0x47, 0xe8};//{0xe4,0x65,0xb8,0xd8,0xd4,0x80};//{0x68,0x25,0xdd,0x32,0x12,0x50};//

constexpr int pinX = 32, pinY = 33;
constexpr int pinL = 5, pinR = 18;
constexpr int pinled = 2;

int round10(int val){return floor(val/10)*10;}


#pragma pack(push,1)
struct message{
  int speedX, speedY;
  bool turnL, turnR;
};
#pragma pack(pop)

ESPnowRimocon<message> rimocon(receiverAddress);

void setup(){
  Serial.begin(115200);
  if(rimocon.init())return;
  pinMode(pinX, ANALOG);
  pinMode(pinY, ANALOG);
  pinMode(pinL, INPUT);
  pinMode(pinR, INPUT);
  pinMode(pinled, OUTPUT);
  digitalWrite(pinled,HIGH);
  delay(1000);
  digitalWrite(pinled,LOW);
}

void loop(){
  digitalWrite(pinled, rimocon.success_send);
  if(!rimocon.success_send)Serial.println("...");
  Serial.print(rimocon.sending.speedX = round10(map(2000-analogRead(pinX),-420,420,-100,100)));Serial.print(",");
  Serial.print(rimocon.sending.speedY = round10(map(2000-analogRead(pinY),-420,420,-100,100)));Serial.print(",");
  Serial.print(rimocon.sending.turnL = !digitalRead(pinL));Serial.print(",");
  Serial.print(rimocon.sending.turnR = !digitalRead(pinR));Serial.println();
  rimocon.send();
  delay(1);
}
