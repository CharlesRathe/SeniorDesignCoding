/********************************************
 * 
 *  Spring 2016 - Senior Design Project
 *  
 *  Team 7: Jake Lanz, Charles Rathe,
 *          Phil Partipilo, Nick Bertram
 *          
 *  Gun Safety Alarm/Controller
 * 
 ********************************************/
 
////////////////////
// Included files //
////////////////////

#include "Keypad.h"
#include "LiquidCrystal.h" 

//////////////////////////
// Variable Definitions //
//////////////////////////  

int addr = 0;          // Start of EEPROM Address
int state = 0;         // Defines state of system
int option = 0;        // Defines what option is selected on menu
boolean pin_valid;     // True if valid pin is entered
boolean pin_set;       // True if a pin has been stored in EEPROM

char entered[4] = {};  // Stores user-entered PIN
char key;
const byte ROWS = 4;   // four rows
const byte COLS = 3;   // three columns

char keys[ROWS][COLS] =  
 {{'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}};

byte rowPins[ROWS] = {
  5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  8, 7, 6}; // connect to the column pinouts of the keypad
  
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal lcd(13, 11, A0, A1, A2, A3);

///////////
// SETUP //
///////////

void setup()
{
  pinMode(3, INPUT); // NOTE: RECIEVER HAS TO BE ON THREE FOR UNO 
    attachInterrupt(digitalPinToInterrupt(3), sound_alarm, RISING);  // SOUNDS ALARM ON RISING ACTION FROM RECIEVER
  getPin(); // Read PIN from EEPROM, if 0x00 then add setpin to menu

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

  if(option == 1){
    lcd.print("<- CHANGE PIN ->");
  }

  if(option == 2){
    lcd.print("<- BATTERY CHK ->");
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
      

      if(key == '4')
      {
        if(option != 0)
            option -= 1;
        else
            option = 2;
      }

      if(key == '#')
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
          }
          selecting = false;
      } 
  } 
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

void change_pin()
{
  // Ask user for set pin
  // Prompt user for new pin
  // Store in EEPROM where pin is stored
  // done 
}

void battery_check()
{
  // SOMEHOW check battery voltage
  // PRINT TO SCREEN
  // DELAY
}

void sound_alarm()
{
  
}
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

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Please select");
  lcd.setCursor(1,1);
  lcd.print("an option");
  delay(1000);
}

char getPin()
{
  char thing;
  // GETS PIN FROM EEPROM
  // RETURNS AS ARRAY
  return char;
}
