/**********************************************************
 * 
 *  GunSafe Security System Transmitter Firmware
 *  Group 10 - Jake Lanz, Charles Rathe
 *             Phil Partipilo, Nick Bertram
 *             
 *             Reads in force sensor (voltage) from adc
 *             Converts it to a string message (3 sig figs)
 *             Uses VirtualWire library to send over RF
 *             
 *             Current Patch:
 *             
 *                - Not tested with receiver yet, but
 *                  sends out correct value over rf
 *                  
 *                - Possible to increase speed withouot 
 *                  conversion although I don't see much
 *                  merit in that
 *                  
 *                - NOTE: Cannot use PWM pins, the VW library 
 *                  fucks with the ones that use Timer1                 
 *                
 **********************************************************/

// Included Libraries
  #include "VirtualWire.h"

// Gloabal Variables
  const float convert = (5.0/1023);   // Constant to convert adc value
  const int outputPin = 12;           // Output pin for rf
  const int led_pin = 4;             // LED flashes when message sent
  
  char msg[4];                        // Variable to send out over VW
  float reading;
  int floatHelper;
  int sensorValue;                    // Variable to store value from force sensor
  int forceDigit;                     // Variable to store MSN of force
  int forceDecimal1;                   // Variable to store decimal of force
  int forceDecimal2;

//////////////////////////////////////////////////////////////////////////////////////
//                                      Setup                                       //
//////////////////////////////////////////////////////////////////////////////////////

void setup() {

// Begin Serial (Debugging)
  Serial.begin(9600);

// Set up Pin I/O
  pinMode(A5, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(led_pin, OUTPUT);

// Set up Virtual Wire
  vwSetup();
}

/////////////////////////////////////////////////////////////////////////////////////
//                                    Main Loop                                    //
/////////////////////////////////////////////////////////////////////////////////////

void loop() {

// Read in sensor value
  sensorValue = analogRead(A5);

// Convert value to voltage string (3 sig figs)
  reading = sensorValue*convert;
  
  forceDigit = (int)reading;
  forceDecimal1 = (reading-forceDigit)*10;
  forceDecimal2 = ((reading-forceDigit)*10-forceDecimal1)*10;
  
  sprintf(msg, "%i.%i%i", forceDigit, forceDecimal1, forceDecimal2);

// Send out message over rf
  digitalWrite(led_pin, HIGH);
  vw_send((uint8_t *)msg, strlen(msg));
  delay(500);
  digitalWrite(led_pin, LOW);

// Print for debug via monitor  
   Serial.println(msg);
}

void vwSetup(){
  
// Invert push-to-talk functionality (not using it)
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(outputPin);

// Set up virtual wire (default output pin is 12)
  vw_setup(2000);
}


