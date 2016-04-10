/************************************************
 * 
 *  GunSafe Security Systems Reciever Testware
 *  Group 10 - Jake Lantz, Charles Rathe
 *             Phil Partiplo, Nick Bertram
 * 
 *             Tests RF with timer interrupts by
 *             reading in messages during interrupts
 *             only
 *             
 *             Current Patch:
 *             
 *                - Can't use TimerOne library as
 *                  Timer1 can't be used at the same 
 *                  time as Virtual Wire
 *                  
 *                - Note: Don't need an analog port for 
 *                  VirtualWire - consider changing for 
 *                  more functionality on adc
 *                  
 ***********************************************************/

// Included Libraries
  #include "VirtualWire.h"
  
// Global Variables
  float reading;

  const int inputPin = 9;

void setup() {
  
// Set up Serial (debugging)
  Serial.begin(9600);

// Set up input pin
  pinMode(inputPin, INPUT);

// Setup Timer0 Interrupt and VirtualWire library
  //setup_Interrupt();
  setup_vw();
  
// Start reciever (uses timer 1)
  vw_rx_start();
 
}

void loop() {

// Delay
  //delay(200); 

  uint8_t buf[4];
  uint8_t buflen = 4;
  char msg[4];
  
// Wait for message
  if(vw_get_message(buf, &buflen)){

  // Print out recieved data (debugging)
    //Serial.print("Got: ");
  
    for(int i=0; i<buflen; i++){
      msg[i] = buf[i];
      //Serial.print(msg[i]);
      //Serial.print(' ');
    }
    
    Serial.println();
    unsigned int value = atoi (msg); 
  Serial.print("Value=");
  Serial.println(value);
  }
  
  
}

void setup_vw(){

// Configure pins
  vw_set_rx_pin(inputPin);
  vw_set_ptt_inverted(true);
  vw_setup(2000);
}

