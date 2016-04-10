/***********************************************************************
*  Program: Control Unit Firmware for the GunSafe EasyUse Locking System 
*
* Authors: Jake Lantz, Charles Rathe,    (software specialty)
*        Phil Partipilo, Nick Bertram  (hardware specialty)
*
* Description: Firmware which recieves input from sensor via  rf transmitter
*        and controls most of the basic functions of 
*        the GunSafe EasyUse Alarm System, including:
*         Keypad -> Using Keypad library
*         LCD -> Using LiquidCrystal library
*         RF Receier -> Using VirtualWire library
*
*
************************************************************************/

// Referenced Libraries
  #include "Keypad.h"
  #include "LiquidCrystal.h" 
  #include "EEPROM.h"
  #include "VirtualWire.h"
  #include "pitches.h"
  #include <Average.h>

// Global Variables
  const int eeAddr = 1;                   // Byte of EEPROM where start of PIN is
  const int digitAddr = 0;                // Byte of EEPROM where number of digits in PIN is stored: if 0, pick PIN
  const int alarmLength_zelda = 54;       // Length of zelda alarm (# of pitches)
  const int alarmLength_two_pitch = 28;   // Length of two pitch alarm
  const int alarmPin = 10;                // Defines PIN which alarm is sent to
  // const int transmit_PIN = 13;               // Defines which PIN recieves data from transmitter
  // const int tiny45_PIN = ???;
  
  int addr;                               // Holds current address
  int STATE = 0;                          // Defines state of system
  int alarmChoice = 0;                    // 0: Two_pitch, 1: Zelda

  int pin_digits;                         // Number of digits in the pin
  int option = 0;                         // Holds current menu option
  int count = 0;                          // Generic counting variable
  int digit = 0;                          // Temporarily holds digits
  
  bool selecting;                         // Determines whether the user is selecting an option
  bool entering;                          // Determines whether the user is entering a PIN
  bool isValid;                           // Return value after validating PIN
  bool alarm_on = false;
  
  char key;                               // Holds characters read from KeyPad
  char newPIN[9];                         // Holds user-entered PIN
  
// Set up different alarms
  const int alarm_two_pitch[] = {NOTE_F7, 0, NOTE_C7, 0, NOTE_F7, 0, NOTE_C7, 0, 
  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,
  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,}; 

  const int alarm_zelda[] = {
  NOTE_G6, 0, NOTE_D6, 0, 
  NOTE_G6, NOTE_G6, NOTE_A6, NOTE_B6, 
  NOTE_C7, NOTE_D7, 0, NOTE_D7,
  NOTE_D7, NOTE_DS7, NOTE_F7, NOTE_G7,  
  0, NOTE_G7, NOTE_G7, NOTE_F7,  
  NOTE_DS7, NOTE_F7, NOTE_DS7,NOTE_D7,
  NOTE_D7, NOTE_C7, NOTE_C7, NOTE_D7,
  NOTE_DS7, NOTE_D7, NOTE_C7, NOTE_AS6,
  NOTE_AS6, NOTE_C7, NOTE_D7,NOTE_C7,
  NOTE_AS6, NOTE_A6, NOTE_A6, NOTE_B6,
  NOTE_CS7, NOTE_E7, NOTE_D7, NOTE_D6,
  NOTE_D6, NOTE_D6, NOTE_D6, NOTE_D6, 
  NOTE_D6, NOTE_D6, NOTE_D6, NOTE_D6,
  NOTE_D6, NOTE_D6,
 };

  const int alarmDurations_zelda[] = {
    6,  9,  4,  8,  
    8,  16, 16, 16,
    16, 2,  8,  8, 
    12, 12, 12, 2,
    8,  8,  12, 12,
    12, 8,  16, 2,
    4,  8,  16, 16,
    2,  8,  8,  8,
    16, 16, 2,  8,
    8,  8,  16, 16,
    2,  4,  8,  16,
    16, 8,  16, 16,
    8,  24, 24, 24,
    8,  4,
  };
  
// Holds the notes of the alarm
  const int alarmDurations_two_pitch[] = {};

  int alarmLength = 28;
 
  int pressureAvg;
  
  const int sampleSize = 25;
  float thresholdValue;
  
// Set up Alarm
  int alarm[] = {NOTE_F7, 0, NOTE_C7, 0, NOTE_F7, 0, NOTE_C7, 0, 
  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,
  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,}; 


  
  
  // Holds the notes of the alarm
  int alarmDurations[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  
  };  // Holds duration of alarm notes

// Set up Keypad and LCD
  const byte ROWS = 4;
  const byte COLS = 3;

  char keys[ROWS][COLS] =  
  {{'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}};

  byte rowPins[ROWS] = {5, 4, 3, 2};
  byte colPins[COLS] = {8, 7, 6};

  Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
  LiquidCrystal lcd(13, 11, A0, A1, A2, A3);

////////////////////////////////////////////////////////////////////
//                          Setup                                 //
////////////////////////////////////////////////////////////////////

void setup(){
  
  // pinMode(transmit_PIN, INPUT);  // Set input pin as an input
  // pinMode(tiny45_PIN, OUTPUT);

  //setup_transmitter();        // Set up transmitter
  setup_timer();              // Set up timer
  
  lcd.begin(16,2);            // Set LCD for 16 columns, 2 lines
  lcd.clear();                // Clear LCD and print intro
  
// Setup in STATE 0

    //Setup for RF transmission **************8
   Serial.begin(9600);  // Debugging only
   vw_set_ptt_inverted(true); // Required for DR3100
   vw_set_rx_pin(12);
   vw_setup(1000);  // Bits per sec
   pinMode(13, OUTPUT);
   vw_rx_start();       // Start the receiver PLL running
   //RF Transmission setup end**************8
   
  lcd.begin(16,2);  // Set LCD for 16 columns, 2 lines
  lcd.clear();      // Clear LCD and print intro

  delay(50);
  state_zero();               // Welcome and prompt user for new pin
}

/////////////////////////////////////////////////////////////////////
//                            Main                                 //
/////////////////////////////////////////////////////////////////////

void loop(){

    menu_state();
}

/////////////////////////////////////////////////////////////////////////
//                          STATE Functions                            //
/////////////////////////////////////////////////////////////////////////

/************
 * STATE 0  |
 ***********/
void state_zero(){
  
// Display Welcome message
    print_welcome();
    
// Read in new PIN # from user
    enterNewPIN();

// Set state to 1
    STATE = 1;

// Print instructions
    print_instructions();
}

/***********
 * STATE 1 |
 **********/
void menu_state(){

  selecting = true;
  option = 0;

  // Print out menu
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("MENU: ");
  
  // Wait for selection
  while(selecting){
    entering = true;
    
  // This option arms or disarms the safe depending on STATE
    if(option == 0){
      lcd.clear();
      lcd.print("MENU:");
      lcd.setCursor(1,1);
      
      if(STATE == 1)
        lcd.print("<-    ARM    ->");
        
      else if(STATE == 2)
        lcd.print("<-   DISARM  ->");
    }

  // This option allows the user to change the PIN
    else if(option == 1){
      lcd.setCursor(1,1);
      lcd.print("<- CHANGEPIN ->");
    }

  // This option allows the user to sound the alarm and check it
    else if(option == 2){
      lcd.setCursor(1,1);
      lcd.print("<- CHECKALARM->");
    }

  // This option allows the user to change the alarm to one of several presets
    else if(option == 3){
      lcd.setCursor(1,1);
      lcd.print("<- CHNGALARM ->");
    }

    else{
      lcd.setCursor(1,1);
      lcd.print("<- CALIBRATE ->");
    }
    
    // Get user selection
     while(entering){
        get_selection();
     }
  }
}

/////////////////////////////////////////////////////////////////////////////////////
//                              Interrupt/Timer Functions                          //
/////////////////////////////////////////////////////////////////////////////////////

void setup_transmitter()
{
  
  
}

void setup_timer(){

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
//  timer_count = 0;
}

ISR(TIMER0_COMPA_vect){

  if(STATE == 1){
    // If analog read is over, go to state 3 (triggered)
  }

  if(STATE == 3){
    // DILEMMA: How do we keep the alarm going while the pin is entered?
    
  }

  // If STATE 0 or 2 do nothing
}

/////////////////////////////////////////////////////////////////////////////////////
//                           Transmission Functions                                //
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
//                                Display Functions                                //
/////////////////////////////////////////////////////////////////////////////////////

void print_welcome(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Welcome!");
  lcd.setCursor(1,1);
  delay(500);
  lcd.print(".  ");
  delay(500);
  lcd.print(".  ");
  delay(500);
  lcd.print(". ");
  delay(500);
}

void print_instructions(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Please select");
  lcd.setCursor(1,1);
  lcd.print("an option");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("4 & 6 to change");
  lcd.setCursor(1,1);
  lcd.print("'#' to select");
}

void print_choose_alarm(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Choose alarm:");
  lcd.setCursor(1,1);
  lcd.print("*-> quit, #-> choose"); 
  delay(2000);
}

void print_arming(){
  lcd.clear();
  lcd.print("Arming");
  delay(500);
  lcd.setCursor(1,1);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
}

void print_disarming(){
  lcd.clear();
  lcd.print("Disarming");
  delay(500);
  lcd.setCursor(1,1);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
}

void print_incorrect_PIN(){
  lcd.clear();
  lcd.print("PIN Incorrect!");
  delay(2000);
}
  
//////////////////////////////////////////////////////////////////////////////
//                            Alarm Functions                               //
//////////////////////////////////////////////////////////////////////////////

void test_alarm(){

  alarm_on = true;
  
  if(alarmChoice == 0)
  {
    for (int thisNote = 0; thisNote < alarmLength_two_pitch; thisNote++) {

      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1400 / alarmDurations_two_pitch[thisNote];
      tone(alarmPin, alarm_two_pitch[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
  
      // stop the tone playing:
      noTone(alarmPin);

      if(!alarm_on){
        STATE = 1;
        thisNote = alarmLength_two_pitch;
      }
        
      //Next note!
    }
  }

  else if(alarmChoice == 1)
  {
    for (int thisNote = 0; thisNote < alarmLength_zelda; thisNote++) {
  
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1400 / alarmDurations_zelda[thisNote];
      tone(alarmPin, alarm_zelda[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
  
      // stop the tone playing:
      noTone(alarmPin);

      if(!alarm_on){
        STATE = 1;
        thisNote = alarmLength_zelda;
      }
      
      //Next note!
    }
  }
}

void change_alarm(){

  selecting = true;
  option = 0;

  print_choose_alarm();

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Choose alarm: ");

  while(selecting){

    entering = true;

    if(option == 0){
      lcd.setCursor(1, 1);
      lcd.print("<- Two Pitch ->");
    }
    
    else if(option == 1){
        lcd.setCursor(1,1);
        lcd.print("<-   Zelda   ->");
    }
  
    while(entering)
      select_alarm();
  }
}

void calibrate_alarm(){

  
}

/////////////////////////////////////////////////////////////////////////////////
//                             PIN Functions                                   //
/////////////////////////////////////////////////////////////////////////////////

bool enterPIN()
{
  // Get number of digits in PIN
  pin_digits = EEPROM.read(digitAddr);

  // Scope Variables
  isValid = true;
  addr = eeAddr;
  entering = true;
  count = 0;

  // Set menu
  lcd.clear();
  lcd.print("Enter PIN");
  lcd.setCursor(1,1);

  // Get PIN
  while(1){
    
      // Get input from keypad
      key = keypad.getKey();

      // Check if a button has been pressed
      if(key != NO_KEY){
        
        // Print the key to the screen
        lcd.print(key);

        if(key == '*'){
          // Reset variables and LCD
          lcd.clear();
          lcd.print("Enter PIN:");
          lcd.setCursor(1,1);
          count = 0;
          
          // Clear array holding pin
          for(int i = 0; i < pin_digits; i++)
            newPIN[i] = 0;
      }

      // User wants to exit 'enter pin' mode
      else if(key == '#')
        return false;

      // If number entered, store it in array
      else{

        digit = key - 48;
        newPIN[count++] = digit;

        // Check if the correct number of digits has been entered
        if(count == pin_digits)
        { 
          for(int i = 0; i < pin_digits; i ++)
          {
            if (!(newPIN[i] == EEPROM.read(addr++)))
              isValid = false;
          }
          return isValid;
        }
      }
    }
  }
}

void enterNewPIN()
{
  // Scope Variables
  addr = eeAddr;
  entering = true;
  count = 0;
  
  lcd.clear();
  lcd.print("Enter # of digits:");
  lcd.setCursor(1,1);

  while(entering)
  {
    // Get input from keypad
    key = keypad.getKey();

    // Check if a button has been pressed
    if(key != NO_KEY)
    {
      // Print the key to the screen
      lcd.print(key);

      if(!(key == '*' || key == '#'))
      {
        // Convert ASCII character to digit and store in EEPROM
        digit = key-48;

        if(digit != 0){
          EEPROM.write(digitAddr, digit);

        // Store new digits locally and exit loop
        pin_digits = digit;
        entering = false;
        }

        else{
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Must be > 0!");
          delay(2000);
        }
      }
    }      
  }

 
  // Reset variables and LCD
  entering = true;
}
void configure_cabinet_pressure()
{
  
  //RF Buffer Vars
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  //Keypad Vars
  boolean entering = true;
  char key;

  Average<int> ave(25);
  
  //Print please close cabinet
  //Print Press * when ready
  lcd.clear();
  lcd.print("Close Cabinet");
  lcd.setCursor(1,1);
  lcd.print("Then Press *");
 
  // While user is still entering
  while(entering)
  {
    // Get input from keypad
    key = keypad.getKey();

    // Check if a button has been pressed
    if(key != NO_KEY)
    {

      // Check if user has pressed '*'
      if(key == '*')
      {
        
        lcd.clear();
        lcd.print("Configuring...");

        //read data in from RF sampleSize times and store in samples[]
        for(int i=0;i < sampleSize;i++)
        {
          
          //Listen to RF, collect sample data
          if (vw_get_message(buf, &buflen)) // Non-blocking
          {

              //add sample to sample array
              //**********MAY NEED SOME CONVERTING INTO DECIMAL NUMBER FROM BUFFER
              ave.push(buf[0]);
              
              
          }//end if

        }//end for

        //take average of array values
        pressureAvg = ave.mean();

        // set threshold to -30%??  of average value?
        thresholdValue = pressureAvg - pressureAvg*(0.3);
        lcd.clear();
        lcd.print("Configured");
        lcd.setCursor(1,1);
        lcd.print(thresholdValue);
 
        entering = false; //clear entering flag
          
   }//end if key ==*       
  }//end if key !=no key
 }//end while(entering)
}//end configure


void disarm()
{
  // Prompt user for PIN
  lcd.clear();
  lcd.print("Enter new PIN:");
  lcd.setCursor(1,1);

  // While user is still entering
  while(entering)
  {
    // Get input from keypad
    key = keypad.getKey();

    // Check if a button has been pressed
    if(key != NO_KEY)
    {
      // Print the key to the screen
      lcd.print(key);

      // Check if user has pressed '*' (clear)
      if(key == '*')
      {
        // Clear bottom line of LCD, reset count
        lcd.clear();
        lcd.print("Enter new PIN:");
        lcd.setCursor(1,1);
        count = 0;

        // Clear array holding pin
        for(int i = 0; i < pin_digits; i++)
        {newPIN[i] = 0;}
      }

      // Check if number entered
      else{

        digit = key - 48;
        // Store in array (increase count)
        newPIN[count++] = digit;

        // Check if the correct number of digits has been entered
        if(count == pin_digits)
        { 
          for(int i = 0; i < pin_digits; i ++)
          {
            EEPROM.write(addr++, newPIN[i]);
          }
          entering = false;
        }
        selecting = false;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//                              Keypad Functions                           //
/////////////////////////////////////////////////////////////////////////////

void get_selection(){

 key = keypad.getKey();
  
  // Check if key is pressed
  if (key != NO_KEY)
  {
    // If key is 6, increment menu
    if(key == '6'){
      if(option != 4)
          option += 1;
      else
          option = 0;
    }
    
    // If key is 4, decrement menu
    else if(key == '4'){
      if(option != 0)
          option -= 1;
      else
          option = 4;
    }

    // If '#' select option
    else if(key == '#')
    {
          if(option == 0){
            if(enterPIN()){

              if(STATE == 1){
                print_arming();
                STATE = 2 ;
              }

              else if(STATE == 2){
                print_disarming();
                STATE = 1;
              }
            }

            else
              print_incorrect_PIN();
          }
            
          else if(option == 1){
            if(enterPIN)
              enterNewPIN();
          }
           
          else if(option == 2){
            test_alarm();}

          else if(option == 3){
            change_alarm();}

          else
            calibrate_alarm();
        

            selecting = false;
    }
    entering = false; 
  }
}

void select_alarm(){

  key = keypad.getKey();
  
  // Check if key is pressed
  if (key != NO_KEY)
  {
    // If key is 6, increment menu
    if(key == '6'){
      if(option == 1)
          option = 0;
      else
          option = 1;
    }
    
    // If key is 4, decrement menu
    else if(key == '4'){
      if(option != 0)
          option = 0;
      else
          option = 1;
    }

    else if(key == '*')
      selecting = false;

    // If '#' select option
    else if(key == '#'){
      if(option == 0)
        alarmChoice = 0;
        
      else if(option == 1)
        alarmChoice = 1;
        
      selecting = false;
    } 
entering = false;
  }
}
