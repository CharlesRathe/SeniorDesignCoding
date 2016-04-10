
//storage variables
boolean toggle = 1;
int timerCount = 0;

void setup() {

  Serial.begin(9600); 
  Serial.println("Setup");
  
  cli();//stop interrupts

//set timer0 interrupt at 61Hz (one per minute)
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 255;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 1024 prescaler
  TCCR0B |= (0 << CS01) | (1 << CS00)| (1 << CS02);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

  
  sei();//allow interrupts
  
}

ISR(TIMER0_COMPA_vect){//timer1 interrupt 61Hz (61 cycels per second)
  if(timerCount == 61)
  {
    timerCount = 0;
    if(toggle)
    {
      Serial.println("Hello");
      toggle = 0;
    }
    else{
      Serial.println("World");
      toggle = 1;
    }
  }
  timerCount++;
}


void loop() {
  

}
