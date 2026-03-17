#include <esp_now.h>
#include <WiFi.h>

#include "Rimocon_ESPNOW.h"

uint8_t receiverAddress[] = {0x68,0x25,0xdd,0x32,0x12,0x50};//{0x80, 0xf3, 0xda, 0x41, 0x47, 0xe8};//
esp_now_peer_info_t peerInfo;

//constexpr int BUTTON_NUM = 2;
//constexpr int VOLUME_NUM = 0;

template<int BOOLNUM = 1, int INTNUM = 1>
struct message{
  static constexpr int _bools = BOOLNUM? BOOLNUM: -1;
  static constexpr int _ints = INTNUM? INTNUM: -1;
  bool digitals[abs(_bools)];
  int analogs[abs(_ints)];
};
message<0,1> data;

void checkArraival(const esp_now_send_info_t* info ,const esp_now_send_status_t flag){
  if(flag==ESP_NOW_SEND_SUCCESS){
    Serial.println("reraly successed!");
  }
  else{
    Serial.println("maybe failed");
  }
}

void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // staitonだけど意味としては端末ってことらしい
  if(esp_now_init() != ESP_OK){
    Serial.println("failed initialize");
    return;
  }
  memcpy(peerInfo.peer_addr, receiverAddress, 6); // MACアドレスの登録(配列なのでmemory copy)
  peerInfo.channel = 0; // よくわからんがチャンネルを自動追従するそうな
  peerInfo.encrypt = false; // 暗号化有無
  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("failed peering");
    return;
  }
  esp_now_register_send_cb(checkArraival);

  pinMode(21, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
}

void loop(){
  //data.digitals[0] = !digitalRead(21);
  //data.digitals[1] = !digitalRead(19);
  data.analogs[0] += data.analogs[0]<100? 1: -100;

  if(esp_now_send(receiverAddress, (uint8_t*)&data, sizeof(data)) == ESP_OK){
    //Serial.println("success sending");
    Serial.println(data.analogs[0]);
    //Serial.print("button:");
    //Serial.println(data.digitals[0]);
  }else{
    Serial.println("failed sending");
  }
  delay(10);
}
