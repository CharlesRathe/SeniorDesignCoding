//this code for transmitter :
//simple Tx on pin D12
//Written By : Mohannad Rawashdeh
// 3:00pm , 13/6/2013
//http://www.genotronex.com/
//..................................
#include <VirtualWire.h>
char *controller;
void setup() {
Serial.begin(9600);
pinMode(13,OUTPUT);
pinMode(0, OUTPUT);
vw_set_ptt_inverted(true); //
vw_set_tx_pin(12);
vw_setup(1000);// speed of data transfer Kbps
}

void loop(){

  // Read the input on analog pin 0:
  int sensorValue = analogRead(A3);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);

  Serial.println(voltage);
  
   // Print out the value you read:
  if(voltage >= .5){
    controller="1";
    digitalWrite(13,1);
  }

  else{

    controller="0";
    digitalWrite(13,0);
  }

   //Send Message
  vw_send((uint8_t *)controller, strlen(controller));
  vw_wait_tx(); // Wait until the whole message is gone

  delay(500);
  
//controller="1";
//vw_send((uint8_t *)controller, strlen(controller));
//vw_wait_tx(); // Wait until the whole message is gone
//digitalWrite(13,1);
//delay(2000);
//controller="0"  ;
//vw_send((uint8_t *)controller, strlen(controller));
//vw_wait_tx(); // Wait until the whole message is gone
//digitalWrite(13,0);
//delay(2000);


}
