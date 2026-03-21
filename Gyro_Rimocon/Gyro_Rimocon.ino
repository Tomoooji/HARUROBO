#include <ESPNOW_Rimocon.h>

constexpr uint8_t receiverAddress[] = {0x68,0x25,0xdd,0x32,0x12,0x50};//{0x80, 0xf3, 0xda, 0x41, 0x47, 0xe8};//

constexpr int pinX = 32, pinY = 33;
constexpr int pinL = ,pinR = ;

#pragma pack(push,1)
struct message{
  int speedX, speedY;
  bool turnL, turnR;
};
#pragma pack(pop)

ESPnowRimocon<message> rimocon(receiverAddress);

void setup(){
  if(rimocon.init())return;
  pinMode(pinX, ANALOG);
  pinMode(pinY, ANALOG);
  pinMode(pinL, INPUT_PULLUP);
  pinMode(pinR, INPUT_PULLUP);
}

void loop(){
  rimocon.sending.speedX = map(2000-analogRead(pinX),-420,420,-100,100);
  rimocon.sending.speedY = map(2000-analogRead(pinY),-420,420,-100,100);
  rimocon.sending.turnL = !digitalRead(pinL);
  rimocon.sending.turnR = !digitalRead(pinL);
  rimocon.send();
}
