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

// Function Definitions
int scroll_count = 0;

// Global Variables
  const int eeAddr = 1;       // Byte of EEPROM where start of PIN is
  const int digitAddr = 0;        // Byte of EEPROM where number of digits in PIN is stored: if 0, pick PIN
  int addr;
  int STATE = 0;            // Defines state of system
//  int alarmLength = 28;
  int alarmLength = 44;
  int alarmPin = 10;

  int pin_digits;       // Number of digits in the pin
  
// Set up Alarm
//  int alarm[] = {NOTE_F7, 0, NOTE_C7, 0, NOTE_F7, 0, NOTE_C7, 0, 
//  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,
//  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,}; 

  int alarm[] = {
  NOTE_D5 , 
  NOTE_F5 , 
   
    NOTE_D6 , 
   
  
    NOTE_D5 , 
   
    NOTE_F5 , 
   
    NOTE_D6 ,
   
  
    NOTE_E6 , 
   
    NOTE_F6 , 
   
    NOTE_E6 , 
   
    NOTE_F6 , 
   
    NOTE_E6 , 
   
    NOTE_C6 , 
   
    NOTE_A5 , 
   
  
    NOTE_A5 , 
   
    NOTE_D5 , 
   
    NOTE_F5 , 
   
    NOTE_G5 , 
   
    NOTE_A5 , 
   
  
    NOTE_A5 , 
   
    NOTE_D5 , 
   
    NOTE_F5 , 
   
    NOTE_G5 , 
   
    NOTE_E5 , 
   
  
  
  
    NOTE_D5 , 
   
    NOTE_F5 , 
   
    NOTE_D6 , 
   
  
    NOTE_D5 , 
   
    NOTE_F5 , 
   
    NOTE_D6 ,
   
  
    NOTE_E6 , 
   
    NOTE_F6 , 
   
    NOTE_E6 , 
   
    NOTE_F6 , 
   
    NOTE_E6 , 
   
    NOTE_C6 , 
   
    NOTE_A5 , 
   
  
    NOTE_A5 , 
   
    NOTE_D5 , 
   
    NOTE_F5 , 
 };
  
  
//  // Holds the notes of the alarm
//  int alarmDurations[] = {
//  12, 12, 12, 12,
//  12, 12, 12, 12,
//  12, 12, 12, 12,
//  12, 12, 12, 12,
// 
//  12, 12, 12, 12,
//  12, 12, 12, 12,
//  12, 12, 12, 12,
//  
//  };  // Holds duration of alarm notes

int alarmDurations[] =
  {8,8,12,8,8,12,12,12,8,8,8,8,8,12,12,8,8,8,8,12,12,8,8,8,8,8,12,8,8,12,12,8,8,8,8,8,8,12,
12,8,8,8,18,12};

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

// Setup in STATE 0
void setup()
{
  lcd.begin(16,2);  // Set LCD for 16 columns, 2 lines
  lcd.clear();      // Clear LCD and print intro
  delay(50);
  print_intro();
}

///////////
// Main  //
///////////

void loop()
{
  if(STATE == 0)
  {
    enterNewPIN();
  }
  
  if(STATE == 1){
    print_menu();
  }

  else if(STATE == 2)
  {
    state_3();
  }
}

///////////////////////////
//    Alarm Functions    //
///////////////////////////

void test_alarm()
{
  
  for (int thisNote = 0; thisNote < alarmLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / alarmDurations[thisNote];
    tone(alarmPin, alarm[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    noTone(alarmPin);
    
    //Next note!
  }
  
}

/////////////////////////////
//    Display Functions    //
/////////////////////////////

int char_to_digit(char c){
  c -= 48;
  return c;
}

void print_intro()
{

// Display Welcome message
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
  
// Read first byte of EEPROM for number of digits
    pin_digits = EEPROM.read(digitAddr);

// If number of digits is 0, prompt enter new PIN
  if(EEPROM.read(digitAddr) == 0)
  {
    lcd.print("Enter New pin:");
    lcd.setCursor(1,1);
    enterNewPIN();
  }

// Else enter PIN stored in EEPROM
  else
    STATE =1;

// Set up menu Screen
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Please select");
  lcd.setCursor(1,1);
  lcd.print("an option");
  delay(2000);

}

void print_menu()
{
  // Boolean to 
  bool selecting = true; // Determines whether the user is selecting an option
  int option = 0;
  char key;

  // Print out menu
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("MENU: ");
  lcd.setCursor(1,1);

  
  // Wait for selection
  while(selecting)
  {

      if(option == 0){
        lcd.setCursor(1,1);
        lcd.print("<- ENTER PIN ->");
      }
    
      else if(option == 1){
        lcd.setCursor(1,1);
        lcd.print("<- CHANGEPIN ->");
      }
    
      else if(option == 2){
        lcd.setCursor(1,1);
        lcd.print("<- CHCKALARM ->");
      }

      key = keypad.getKey();
  
      // Check if key is pressed
      if (key != NO_KEY)
      {
        // If key is 6, increment menu
        if(key == '6')
        {
          if(option != 2)
              option += 1;
          else
              option = 0;
        }
        
        // If key is 4, decrement menu
        else if(key == '4')
        {
          if(option != 0)
              option -= 1;
          else
              option = 2;

        }
  
        // If '#' select option
        else if(key == '#')
        {
              if(option == 0)
                disarm();
                
              else if(option == 1)
                change_pin();
               
              else if(option == 2)
                test_alarm();
                
            selecting = false;
   
        } 
      }
  }
}

/////////////////////////
//    PIN Functions    //
/////////////////////////

bool verify_PIN(int newPIN[])
{
  pin_digits = EEPROM.read(digitAddr);
  // Boolean to return
  boolean isValid = true;

  // Array to store PIN
  char digit;

  addr = eeAddr;

  // Load the pin from memory, compare to user-entered pin
  for(int i = 0; i < pin_digits; i++)
  {
    digit = EEPROM.read(addr++);
    if(!(newPIN[i] == digit)) {isValid = false;}
  }
  
  return isValid;


}
void enterNewPIN()
{
  // Scope Variables
  addr = eeAddr;
  char key;
  bool entering = true;
  int count = 0;
  int char_to_digit;
  char newPIN[9] = {};
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
          char_to_digit = key;
          char_to_digit -= 48;
          EEPROM.write(digitAddr, char_to_digit);

          // Store new digits locally and exit loop
          pin_digits = char_to_digit;
          entering = false;
        }
      }      
  }

    // Reset variables and LCD
    entering = true;
    lcd.clear();
    lcd.print("Enter new PIN");
    lcd.setCursor(1,1);
    count = 0;

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
        lcd.print("New pin:");
        lcd.setCursor(1,1);
        count = 0;

        // Clear array holding pin
        for(int i = 0; i < pin_digits; i++)
        {newPIN[i] = 0;}
      }

      // Check if number entered
      else{

        int digit = key -= 48;
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
          STATE = 1;
        }
      }
    }
  }
}

void change_pin()
{
  lcd.clear();
  lcd.print("Enter PIN: ");
  lcd.setCursor(1,1);
  
  int count =0;
  bool entering = true;
  char key;
  int newPIN[9];
  
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
        lcd.print("Enter PIN:");
        lcd.setCursor(1,1);
        count = 0;

        // Clear array holding pin
        for(int i = 0; i < pin_digits; i++)
        {newPIN[i] = 0;}
      }

      // Exit back to menu if user enters '#'
      if(key == '#')
      {
        entering = false;
      }

      // Check if number entered
      else{

        // Store in array (increase count)
        newPIN[count++] = char_to_digit(key);

        // Check if the correct number of digits has been entered
        if(count == pin_digits)
        { 
          if(verify_PIN(newPIN))
          {
            entering = false;
            STATE = 0;
          }

          else{
            // Tell user PIN is incorrect
            lcd.clear();
            lcd.print("Incorrect!");

            // Reset variables
            for(int i = 0; i < pin_digits; i++)
            {newPIN[i] = 0;}
            count = 0;
            lcd.clear();
            lcd.print("Enter PIN: ");
            lcd.setCursor(1,1);
          }
        }
      }
    }
  }
}

void disarm()
{
  // Prompt user for PIN
  lcd.clear();
  lcd.print("Enter PIN:");
  lcd.setCursor(1,1);

  boolean entering = true;
  int count = 0;
  char key;
  int newPIN[9];

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
        lcd.print("Enter PIN:");
        lcd.setCursor(1,1);
        count = 0;

        // Clear array holding pin
        for(int i = 0; i < pin_digits; i++)
        {newPIN[i] = 0;}
      }

      // Exit back to menu if user enters '#'
      if(key == '#')
      {
        entering = false;
      }

      // Check if number entered
      else{

        // Store in array (increase count)
        newPIN[count++] = char_to_digit(key);

        // Check if the correct number of digits has been entered
        if(count == pin_digits)
        { 
          if(verify_PIN(newPIN))
          {
            entering = false;
            lcd.clear();
            lcd.print("Disarmed!");
            STATE = 2;
          }

          else{
            // Tell user PIN is incorrect
            lcd.clear();
            lcd.print("Incorrect!");

            // Reset variables
            for(int i = 0; i < pin_digits; i++)
            {newPIN[i] = 0;}
            count = 0;
            lcd.clear();
            lcd.print("Enter PIN: ");
            lcd.setCursor(1,1);
          }
        }
      }
    }
  }
}

void state_3(){

  bool disarmed = true;
  char key;
  int count = 0;
  int newPIN[9];
  bool entering = true;

  lcd.clear();
  lcd.print("Enter PIN to arm (* to clear)");
  lcd.setCursor(1,1);
  while(disarmed){

     key = keypad.getKey();
  
     if(key != NO_KEY)
    {
      // Print the key to the screen
      lcd.print(key);

      // Check if user has pressed '*' (clear)
      if(key == '*')
      {
        // Clear bottom line of LCD, reset count
        lcd.clear();
        lcd.print("Enter PIN to arm (* to clear):");
        lcd.setCursor(1,1);
        count = 0;

        // Clear array holding pin
        for(int i = 0; i < pin_digits; i++)
        {newPIN[i] = 0;}
      }

      // Exit back to menu if user enters '#'
      if(key == '#')
      {
        entering = false;
      }

      // Check if number entered
      else{

        // Store in array (increase count)
        newPIN[count++] = char_to_digit(key);

        // Check if the correct number of digits has been entered
        if(count == pin_digits)
        { 
          if(verify_PIN(newPIN))
          {
            entering = false;
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
            disarmed = false;
            STATE = 1;
          }

          else{
            // Tell user PIN is incorrect
            lcd.clear();
            lcd.print("Incorrect!");

            // Reset variables
            for(int i = 0; i < pin_digits; i++)
            {newPIN[i] = 0;}
            count = 0;
            lcd.clear();
            lcd.print("Enter PIN to arm (* to clear): ");
            lcd.setCursor(1,1);
          }
        }
      }
    }
  }
}

