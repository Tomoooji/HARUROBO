//timer button class for ps4(and other) controller
#pragma once

class Timer{
 public:
  const int _time;
  unsigned long last;
  bool plessed;
  bool state;
  Timer(const int time):_time(time){}
  void begin(){
    this->last = millis();
  }
  bool isPushed(){
    if(this->plessed){
      if(!state){
        if(millis()-this->last > this->_time){
          this->state = true;
          //Serial.println("Switch");
          return true;
        }else{
          //Serial.println("Cancel");
        }
      }else{
          //Serial.println("Pleese");
      }
    }else{
      this->isReleased();
    }
    return false;
  }
  bool isReleased(){
    if((!this->plessed) && this->state){
      this->last = millis();
      this->state = false;
      //Serial.println("Release");
      return true;
    }else{ // free
      return false;
    }
  }
};
