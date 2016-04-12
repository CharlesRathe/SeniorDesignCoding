 /***************** 
  *  Standalone Tiny45
  *  Speaker IC
  *****************/
  
// Variables
const int alarmPin = 0;                // Defines PIN which alarm is sent to
const int inputPin = 1;
const int tone_delay = 2270;

void setup() {
  // Set pin I/O
  pinMode(alarmPin, OUTPUT);
  pinMode(inputPin, INPUT);
  
}

void loop() {
  if(digitalRead(inputPin) == HIGH){
      digitalWrite(alarmPin, HIGH);
      delayMicroseconds(tone_delay);
      digitalWrite(alarmPin, LOW);
      delayMicroseconds(tone_delay); 
  }
}

