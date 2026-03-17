// base class for send/recieve data by esp-now
#pragma once
#include <esp_now.h>
#include <WiFi.h>

template <typename DATATYPE>
class Rimocon{
  esp_now_peer_info_t _peer_info;
 public:
  uint8_t* receiver_address;
  volatile bool success_send;
  DATATYPE sending;
  volatile bool receive_new;
  volatile DATATYPE received;

  static Rimocon *_instance = nullptr;

  Rimocon(const uint8_t* address = nullptr):receiver_address(address){}
  
  bool init(const uint8_t* address = nullptr){
    // recomend making bool begin(); and call this from childe class
    WiFi.mode(WIFI_STA);
    if(esp_now_init() != ESP_OK) return false;
    if(address != nullptr) this->receiver_address = address;
    if(this->receiver_address != nullptr){
      memset(&this->_peer_info, 0, sizeof(this->_peer_info));
      memcpy(this->_peer_info.peer_addr, this->receiver_address, 6);
      this->_peer_info.channel = 0; // よくわからんがチャンネルを自動追従するそうな
      this->_peer_info.encrypt = false; // 暗号化有無
      if(esp_now_add_peer(&this->_peer_info) != ESP_OK){
        //Serial.println("failed peering");
        return false;
      }
    }
    _instance = this;
    esp_now_register_send_cb(static_send_cb);
    esp_now_register_recv_cb(static_recv_cb);
    return true;
  }

  static void static_send_cb(const esp_now_send_info_t* info ,const esp_now_send_status_t flag){
    _instance->success_send = (flag == ESP_NOW_SEND_SUCCESS);
  }

  static void static_recv_cb(const esp_now_recv_info_t* info, const uint8_t* data, int len){
    if(_instance->receive_new || sizeof(_instance->recieved) != len)return;
    memcpy(&_instance->received, data, sizeof(_instance->received));
    _instance->receive_new = true;
  }

  void send(){
    esp_now_send(this->receiver_address, (uint8_t*)&this->sending, sizeof(this->sending));
  }

};

//inline Rimocon* Rimocon::_instance = nullptr;
