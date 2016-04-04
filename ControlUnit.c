/***********************************************************************
*	Program: Control Unit Firmware for the GunSafe EasyUse Locking System 
*
*	Authors: Jake Lantz, Charles Rathe,    (software specialty)
*		     Phil Partipilo, Nick Bertram  (hardware specialty)
*
*	Description: Firmware which recieves input from sensor via  rf transmitter
*				 and controls most of the basic functions of 
*				 the GunSafe EasyUse Alarm System, including:
*					Keypad -> Using Keypad library
*					LCD -> Using LiquidCrystal library
*					RF Receier -> Using VirtualWire library
*
*
************************************************************************/

// Referenced Libraries
	#include "Keypad.h"
	#include "LiquidCrystal.h" 
	#include "VirtualWire.h"
	#include "pitches.h"

// Global Variables
	int eeAddr = 1; 			// Byte of EEPROM where start of PIN is
	int digitAddr = 0;       	// Byte of EEPROM where number of digits in PIN is stored: if 0, pick PIN
	int STATE = 0;         		// Defines state of system

	int pin_digits;				// Number of digits in the pin
	bool pin_valid;     		// True if valid pin is entered
	bool pin_set;       		// True if a pin has been stored in EEPROM

  int AlarmLength = 28;  //Need to know number of notes in alarm[]
  int AlarmPin = 10;
	// char key; Different scope
	// int option; Different scope
	// char entered[4] = {};  	// SCOPE

// Set up Alarm
	int alarm[] = {NOTE_F7, 0, NOTE_C7, 0, NOTE_F7, 0, NOTE_C7, 0, 
  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,
  NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,};      		// Holds the notes of the alarm
	
  int alarmDurations[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  
  };  // Holds duration of alarm notes

	char memPIN[4] = {};   		// Reserve array to hold PIN from EEPROM
	const char hardCodePIN =	// Hardcoded PIN for testing (comment out later)
		{'1', '2', '3', '4'};

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
  print_menu();
}

bool validate_pin()
{
  boolean isValid = true;
  
  for(int i = 0; i <=3; i++)
  {
    if(!(PIN[i] == entered[i]))
    {
      isValid = false;
    }
  }

  if(isValid)
  {
    return true;
  }

  else
  { return false;}
}


///////////////////////////
//    Alarm Functions    //
///////////////////////////

void sound_alarm()
{
  for (int thisNote = 0; thisNote < AlarmLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / alarmDurations[thisNote];
    tone(AlarmPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    noTone(AlarmPin);
    
    //Next note!
  }
  
}

/////////////////////////////
//    Display Functions    //
/////////////////////////////

void print_intro()
{
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Welcome!");
  lcd.setCursor(1,1);
  delay(500);
  lcd.print(".  ");
  delay(500);
  lcd.print(". ");
    delay(500);
  lcd.print(". ");

  pin_digits = EEPROM.read();

  if(pin_digits == 0)
  {
  	lcd.print("Enter New pin:");
  	lcd.setCursor(1,1);
  	enterNewPIN();
  }

  else{

  	lcd.print("Enter your pin:");
  	lcd.setCursor(1,1);
  	if(!enterPIN())
  	{


  	}
  }

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Please select");
  lcd.setCursor(1,1);
  lcd.print("an option");
  delay(1000);
}

void print_menu()
{
  boolean selecting = true; // Determines whether the user is selecting an option
  
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("MENU: ");
  lcd.setCursor(1,1);

  if(option == 0){
    lcd.print("<- ENTER PIN ->");
  }

  else if(option == 1){
    lcd.print("<- CHANGE PIN ->");
  }

  else if(option == 2){
    lcd.print("<- CHECKALARM ->");
  }

  while(selecting == true);
  {
    key = keypad.getKey();

    if (key != NO_KEY)
    {
      if(key == '6')
      {
        if(option != 2)
            option += 1;
        else
            option = 0;
      }
      

      else if(key == '4')
      {
        if(option != 0)
            option -= 1;
        else
            option = 2;
      }

      else if(key == '#')
      {
          switch(option)
          {
            case '0':
              enterPIN();
              break;
            case '1':
              change_pin();
              break;
            case '2':
              battery_check();
              break;
          }
          selecting = false;
      } 
    }
  } 
}


/////////////////////////
//    PIN Functions    //
/////////////////////////

void enterNewPIN()
{


}

bool enterPIN()
{


}

void change_pin()
{
  // Ask user for set pin
  // Prompt user for new pin
  // Store in EEPROM where pin is stored
  // done 
}


void enterPIN()
{
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Enter your PIN('#' to exit)");  // REMEMBER TO SET TO SCROLL
  lcd.setCursor(1,1);
  
  boolean entering = true;
  int count = 0;
  
  while(entering){
    
    key = keypad.getKey();
    
    if (key != NO_KEY)
    {
      lcd.print(key);
      entered[count] = key;
      count++;
  
      if(key == '#')
      {
        entering = false;
      }
      
      if(count == 4)
      {
        pin_valid = validate_pin();
      
        if (pin_valid)
        {
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("DISARMED!");
          delay(1000);
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Enter your PIN ('#' to exit)");
          lcd.setCursor(1,1);
          
          //Serial.println("\nCorrect!");
          count=0;
        }
    
        else
        {
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("INCORRECT PIN!");
          delay(1000);
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Enter your PIN ('#' to exit)");
          lcd.setCursor(1,1);
          count=0;
        }
      }
    }
  }
}

bool validate_pin()
{
  boolean isValid = true;
  
  for(int i = 0; i <=3; i++)
  {
    if(!(PIN[i] == entered[i]))
    {
      isValid = false;
    }
  }

  if(isValid)
  {
    return true;
  }

  else
  { return false;}
}
