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
	const int eeAddr = 1; 			// Byte of EEPROM where start of PIN is
	const int digitAddr = 0;       	// Byte of EEPROM where number of digits in PIN is stored: if 0, pick PIN
	int addr;
	int STATE = 0;         		// Defines state of system

	int pin_digits;				// Number of digits in the pin
	bool pin_valid;     		// True if valid pin is entered
	bool pin_set;       		// True if a pin has been stored in EEPROM

	// char key; Different scope
	// int option; Different scope
	// char entered[4] = {};  	// SCOPE

// Set up Alarm
	int alarm[] = {};      		// Holds the notes of the alarm
	int alarmDurations[] = {};  // Holds duration of alarm notes

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
  //print_menu();
	lcd.print("1");
	delay(5000);
}

///////////////////////////
//    Alarm Functions    //
///////////////////////////

void sound_alarm()
{
  
}

/////////////////////////////
//    Display Functions    //
/////////////////////////////

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
	lcd.print(". ");
	delay(500);
	lcd.print(". ");

// Read first byte of EEPROM for number of digits
  	pin_digits = EEPROM.read(digitAddr);

// If number of digits is 0, prompt enter new PIN
	if(pin_digits == 0)
	{
		lcd.print("Enter New pin:");
		lcd.setCursor(1,1);
		enterNewPIN();
	}

// Else enter PIN stored in EEPROM
	else{enterNewPIN();}

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
    lcd.print("<- CHECK ALARM ->");
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
	// Print out instructions to screen
	lcd.print("New pin (* to clear):");
	lcd.setCursor(1,1);

	// Scope Variables
	char key;
	bool entering = true;
	int count = 0;
	char[pin_digits] newPIN = {};

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
				lcd.print("New pin (* to clear):");
				lcd.setCursor(1,1);
				count = 0;

				// Clear array holding pin
				for(int i = 0; i < pin_digits; i++)
				{newPIN[i] = 0;}
			}

			// Check if number entered
			else{

				// Store in array (increase count)
				newPIN[count] = key;
				count++;

				// Check if the correct number of digits has been entered
				if(count == pin_digits-1)
				{	
					// Verify if PIN is correct, if it is exit setup
					entering = !verifyPIN(newPIN);
				}
			}
		}
	}
}

void change_pin()
{
  // Ask user for set pin
  // Prompt user for new pin
  // Store in EEPROM where pin is stored
  // done 
}


// void enterPIN()
// {
//   lcd.clear();
//   lcd.setCursor(1,0);
//   lcd.print("Enter your PIN('#' to exit)");  // REMEMBER TO SET TO SCROLL
//   lcd.setCursor(1,1);
  
//   boolean entering = true;
//   int count = 0;
  
//   while(entering){
    
//     key = keypad.getKey();
    
//     if (key != NO_KEY)
//     {
//       lcd.print(key);
//       entered[count] = key;
//       count++;
  
//       if(key == '#')
//       {
//         entering = false;
//       }
      
//       if(count == 4)
//       {
//         pin_valid = validate_pin();
      
//         if (pin_valid)
//         {
//           lcd.clear();
//           lcd.setCursor(1,0);
//           lcd.print("DISARMED!");
//           delay(1000);
//           lcd.clear();
//           lcd.setCursor(1,0);
//           lcd.print("Enter your PIN ('#' to exit)");
//           lcd.setCursor(1,1);
          
//           //Serial.println("\nCorrect!");
//           count=0;
//         }
    
//         else
//         {
//           lcd.clear();
//           lcd.setCursor(1,0);
//           lcd.print("INCORRECT PIN!");
//           delay(1000);
//           lcd.clear();
//           lcd.setCursor(1,0);
//           lcd.print("Enter your PIN ('#' to exit)");
//           lcd.setCursor(1,1);
//           count=0;
//         }
//       }
//     }
//   }
// }

bool verify_pin(char[] newPIN)
{
	// Boolean to return
	boolean isValid = true;

	// Array to store PIN
	char digit;

	addr = eeAddr;

	// Load the pin from memory, compare to user-entered pin
	for(i = 0; i < pin_digits; i++)
	{
		digit = EEPROM.read(addr++);
		if(!(newPIN[i] == digit)) {isValid = false;}
	}
  
	return isValid;
}
