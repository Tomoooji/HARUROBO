#pragma once

#pragma pack(1)
struct Payload{
  uint8_t header; // 0xAA55
  bool sw;
  int16_t pots[13];
};
#pragma pack()

Payload data;
const size_t payloadSize = sizeof(Payload);

/*
if(Serial2.available() >= payloadSize){
  uint16_t checkHead;
  Serial2.readBites((uint8_t*)& checkHead, 2);
  if(checkHead == 0xAA55){
    Serial2.readBytes((uint8_t*)&data + 2, payloadSize - 2);
  }
  else{
    Serial
  }
}
*/