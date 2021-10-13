#include <Wire.h>
#include <stdint.h>

enum state = {WAITING_COMMAND,WAITING_COMMAND_DATA}
uint8_t valve;


void setup() {
  for(int i = 40; i<51;i++){
     pinMode(i, OUTPUT);
  }
  Wire.begin(0x20);        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  //Setup the state
  state = WAITING_COMMAND;
  Wire.onReceive(onReceiveCommand);
  
}



void onReceiveCommand(){

  //Read the octet on the wire
  uint8_t octet = Wire.read();

  //State mahine, initial state => WATING_COMMAND
  switch(state){
    case WAITING_COMMAND:
      valve = octet;
      state = WAITING_COMMAND_DATA;
      break;
      
    case WATING_COMMAND_DATA:
      if(octet){
        if(valve >= 40 && valve<= 51){
          //True => >0 => ouvrir la valve
          //Sanity check on the pin value
          digitalWrite(valve,HIGH);
        }
      }
      else if(valve >= 40 && valve<= 51)){
        digitalWrite(valve,LOW);
      }
  }
}
