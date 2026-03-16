#include <esp_now.h>

#include <WiFi.h>
//68:25:dd:32:12:50
volatile bool getlatest = false;

template<int BOOLNUM = 1, int INTNUM = 1>
struct message{
  bool digitals[BOOLNUM];
  int analogs[INTNUM];
};
message<2,1> InputData;

void OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len){
  memcpy(&InputData, data, sizeof(InputData));
  getlatest = true;
  //Serial.println("success reciving");
}
void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // staitonだけど意味としては端末ってことらしい
  if(esp_now_init() != ESP_OK){
    Serial.println("failed initialize");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  ledcAttach(2,12800,8);
  pinMode(17,OUTPUT);
}

void loop(){
  Serial.println(getlatest);
  if(getlatest){
    //Serial.println(InputData.analogs[0]);
    Serial.println(InputData.digitals[0]);
    ledcWrite(2, InputData.analogs[0]);
    getlatest = false;
    digitalWrite(17, LOW);
  }else{
    ledcWrite(2,0);
    digitalWrite(17, HIGH);
    //Serial.println("cannot detect message");
  }
  //delay(100);
}