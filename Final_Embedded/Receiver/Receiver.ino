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
************************************************************************/

// Referenced Libraries
  #include "Keypad.h"
  #include "LiquidCrystal.h" 
  #include "EEPROM.h"
  #include "VirtualWire.h"
  #include "pitches.h"
  #include <SPI.h>
  #include <Ethernet.h>
  //#include "FPS_GT511C3.h"
  //#include "SoftwareSerial.h"

// Global Variables
  const int eeAddr = 1;                   // Byte of EEPROM where start of PIN is
  const int digitAddr = 0;                // Byte of EEPROM where number of digits in PIN is stored: if 0, pick PIN
  const int rxPin = A4;                    // Defines which PIN recieves data from transmitter
  const int alarmPin = 10;                // Defines PIN which alarm is sent to
  
  const float defaultTH = 1;             // Default pressure threshhold for alarm (V)
  
  int addr;                               // Holds current address
  int STATE = 0;                          // Defines state of system (Disarmed -> 1)
  int pin_digits;                         // Number of digits in the pin
  int option = 0;                         // Holds current menu option
  int count = 0;                          // Generic counting variable
  int digit = 0;                          // Temporarily holds digits
  int currentTH = 2;                     // Current threshhold set by user (V)
  int reading;                            // Holds reading from rf

  //int backlight_pin = ??;               //toggle backlight pin for power saving
  
  float calibratedTH;                     //Holds the calibrated threshold value from rf
  float thresholdPercent = 0.25;          //Determines what threshold below our sampled pressure is allowed
  int sampleSize = 25;                    //Determines how many samples to take for threshold

  bool selecting;                         // Determines whether the user is selecting an option
  bool entering;                          // Determines whether the user is entering a PIN
  bool isValid;                           // Return value after validating PIN
  bool text = false;
  
  char key;                               // Holds characters read from KeyPad
  char newPIN[9];                         // Holds user-entered PIN

// Set up Keypad and LCD
  const byte ROWS = 4;
  const byte COLS = 3;

//Ethernet variables
    // Enter a MAC address for your controller below.
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    //thingspeak server
    char server[] = "api.thingspeak.com";

    //if DHCP fails, use a static IP
    IPAddress ip(192,168,0,177);
    // Initialize the Ethernet client library
    EthernetClient client;

    //API key for the Thingspeak ThingHTTP already configured
    const String apiKey = "F5RZUI95MVSKQBG0";

    //the number the message should be sent to
    const String sendNumber = "";


  char keys[ROWS][COLS] =  
  {{'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}};

 // byte rowPins[ROWS] = {5, 4, 3, 2};
  //byte colPins[COLS] = {8, 7, 6};
  byte rowPins[ROWS] = {9, 8, 7, 6};
  byte colPins[COLS] = {5, 2, 3};
 // FPS_GT511C3 fps(3, 2);
  Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
  LiquidCrystal lcd(13, 11, A0, A1, A2, A3);
  //LiquidCrystal lcd(A4, A5, A0, A1, A2, A3);
////////////////////////////////////////////////////////////////////
//                          Setup                                 //
////////////////////////////////////////////////////////////////////

void setup(){
  
// Set up Serial (debugging)
  //Serial.begin(9600);
  // setupEthernet();

   
// Set up pin I/O
  pinMode(rxPin, INPUT);  
  pinMode(alarmPin, OUTPUT);
  digitalWrite(alarmPin, LOW);

// Set up RF Transmitter and Timer 0
  //setup_transmitter(); 
  digitalWrite(alarmPin, LOW);

 

//// Set up LCD
  lcd.begin(16,2);            // Set LCD for 16 columns, 2 lines
  lcd.clear();                // Clear LCD and print intro
  

  delay(1000); //delay to put rx and tx back in if needed
// Set up FPS
 // setupFPS();

// Short delay, then go to setup state
  delay(50);
  state_zero();  

}

/////////////////////////////////////////////////////////////////////
//                            Main                                 //
/////////////////////////////////////////////////////////////////////

void loop(){

// If not alarm state, go to menu
  if(STATE != 3)
    menu_state();
    
// Else, Alarm ON, need to turn off alarm
  else{
    
    if(enterPIN()){
      STATE = 1;
      digitalWrite(alarmPin, LOW);
    }
  }
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
    
    else if(option == 3){
      lcd.setCursor(1,1);
      lcd.print("<-TOGGLE TEXT->");
    }

  // This option allows the user to change the alarm to one of several presets
    else if(option == 4){
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
//                           Transmission Functions                                //
/////////////////////////////////////////////////////////////////////////////////////
bool check_pressure(){
//  bool pressure = true;
//  
//  if(STATE == 2){
//    Serial.println(analogRead(A4));
//    if(analogRead(A4) < currentTH){
//      pressure = false;
//    }

//    uint8_t buf[4];
//    uint8_t buflen = 4;
//    String msg = "";
    
//    vw_rx_start();
//
//   if(vw_wait_rx_max(1000)) //wait at max 1 second for a sample
//   { 
//    
//      if(vw_get_message(buf, &buflen)){
//          
//        // Print out recieved data (debugging)
//          //Serial.print("Got: ");
//      
//          for(int i=0; i<buflen; i++){
//            msg += (char) buf[i];
//            //Serial.print(msg[i]);
//            //Serial.print(' ');
//          }
//  
//          //Serial.print("    String: ");
//          //Serial.print(msg);
//  
//           if(msg.toFloat() < calibratedTH)
//              pressure = false;
//      
//          //Serial.println();
//        }
// }
  
  return true;
}

//get a pressure value from the transmitter
float get_pressure()
{

//    uint8_t buf[4];
//    uint8_t buflen = 4;
//    String msg = "";
    float pressureSample;
    
//    vw_rx_start();
//
//    if(vw_wait_rx_max(1000) ) //wait at max 1 second for a sample
//    {
//      if(vw_get_message(buf, &buflen) )
//      {
//        
//      // Print out recieved data (debugging)
//        //Serial.print("Got: ");
//    
//        for(int i=0; i<buflen; i++){
//          msg += (char) buf[i];
//          //Serial.print(msg[i]);
//          //Serial.print(' ');
//        }

        //Serial.print("    String: ");
        //Serial.print(msg);

//         pressureSample = msg.toFloat();
            pressureSample = 10;
    
    
        //Serial.println();
      return pressureSample;
//    else{
//         print_calibration_error();
//         //Serial.print("No value from sensor");
//         STATE = 1;
//         menu_state();
//    }
    

  
}


//void setup_transmitter(){
//
//// Configure pins/settings
//  vw_set_rx_pin(rxPin);
//  vw_set_ptt_inverted(true);
//  vw_setup(2000);
//}

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

void print_disable_alarm(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(" ***TESTING*** ");
  lcd.setCursor(1,1);
  lcd.print("  '#' to stop  "); 
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

void print_calbrating(){
  lcd.clear();
  lcd.print("Calibrating");
  delay(500);
  lcd.setCursor(1,1);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
  lcd.print(". ");
  delay(500);
}

void print_calibrated(){
  lcd.clear();
  lcd.print("Calibration");
  lcd.setCursor(1,1);
  lcd.print("Complete");
  delay(2000);
}

void print_calibration_error(){
  lcd.clear();
  lcd.print("Calibration");
  lcd.setCursor(1,1);
  lcd.print("Error");
  ////Serial.println("Receiver Error");
  delay(2000);
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

    print_disable_alarm();
    delay(100);
    digitalWrite(alarmPin, HIGH);
    bool testing = true;
    if(text){
      textAlarm();
    }

    while(testing){

      // Get input from keypad
      key = keypad.getKey();

      // If input is '#' -> Stop testing
      if(key != NO_KEY){
        if(key == '#')
          testing = false;  
      }
    }
    
    digitalWrite(alarmPin, LOW);
}

void calibrate_alarm(){
//
//  float sampleTmp;                 //float to hold a sample pressure value from rf
//  float sampleSum=0;               //float to hold sum of all gathered samples
//
//
//  //Display calibrating in progress
//  print_calbrating();               
//
//  //Gather samples, keep record of total sum for average computation
//  for(int i=0;i<sampleSize;i++)
//  {
//      //Serial.print("Sample:");
//      //Serial.println(i);
//      sampleTmp = get_pressure();
//      sampleSum = sampleSum + sampleTmp;
//  }
//
//  //Compute threshold value - total sampleSum / total sampleSize - threshold percent
//  calibratedTH = (sampleSum / sampleSize) - (  (sampleSum / sampleSize) * thresholdPercent );

  //Print calibration value - debugging
  //Serial.print("Calibrated: ");
  //Serial.println(calibratedTH);

  //Display calibrated complete
  print_calibrated();
    
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

        if(digit >= 1){
          EEPROM.write(digitAddr, digit);

        // Store new digits locally and exit loop
        pin_digits = digit;
        entering = false;
        }

        else{
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Must be > 1!");
          delay(2000);
        }
      }
    }      
  }

  // Reset variables and LCD
  entering = true;
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

  if(!check_pressure()){
    digitalWrite(alarmPin, HIGH);
    STATE = 3;
    selecting = false; 
    entering = false;
    //Serial.print("ALARM");
    if(text){
      textAlarm();
    }
  }

 key = keypad.getKey();
  
  // Check if key is pressed
  if (key != NO_KEY)
  {
    // If key is 6, increment menu
    if(key == '6'){
      if(STATE == 2){
        if(option != 3)
            option++;
        else
            option = 0;
      }
      else{
        if(option != 4)
          option++;
        else
          option=0;
        }
    }
    
    // If key is 4, decrement menu
    else if(key == '4'){
      if(STATE == 1){
        if(option != 0)
            option -= 1;
        else
            option = 4;
      }

      else{
        if(option!=0)
          option -= 1;
        else
          option = 3;
      }
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
                digitalWrite(alarmPin, LOW);
              }
            }

            else
              print_incorrect_PIN();
          }
            
          else if(option == 1){
           if(enterPIN())
              enterNewPIN();
           else{
              lcd.clear();
              lcd.print("Incorrect");
              lcd.setCursor(1,1);
              lcd.print("Pin");
              delay(2000);
           }
          }
         
           
          else if(option == 2){
            test_alarm();}

          else if(option == 3)
            calibrate_alarm();

          else if(option == 4){
            text = !text;
          }
          
            selecting = false;
    }
    entering = false; 
  }
}

void textAlarm()
{
  //setupEthernet();

 // Make a TCP connection to remote host
  if (client.connect(server, 80))
  {

    //should look like this...
    //api.thingspeak.com/apps/thinghttp/send_request?api_key={api key}
    //configure the message in twilio account

    client.print("POST /apps/thinghttp/send_request?api_key=");
    client.print(apiKey);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    //Serial.println(F("Connection failed"));
  } 

  // Check for a response from the server, and route it
  // out the serial port.
  while (client.connected())
  {
    if ( client.available() )
    {
      char c = client.read();
      //Serial.print(c);
    }      
  }
  //Serial.println();
  client.stop();

}
void setupEthernet()
{
  //Serial.println("Setting up Ethernet...");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    //Serial.println(F("Failed to configure Ethernet using DHCP"));
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  //Serial.print("My IP address: ");
  //Serial.println(Ethernet.localIP());
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

//void setupFPS()
//{
//  fps.UseSerialDebug = false;
//  delay(1000);
////  lcd.clear();
////lcd.setCursor(1,0);
////  lcd.print("Open FPS");
//  
//  fps.Open();
//  delay(1000);
//  blinkyFPS();
////  lcd.clear();
////lcd.setCursor(1,0);
////  lcd.print("HURRAY");
////  fps.SetLED(true); 
//}

//boolean checkFingerPrint()
//{
//  boolean fingerPrint; //True if valid, false if not
//  
//  // Identify fingerprint test
//  if (fps.IsPressFinger())
//  {
//    fps.CaptureFinger(false);
//    int id = fps.Identify1_N();
//    if (id <200)
//    {
//      //Serial.print("Verified ID:");
//      //Serial.println(id);
//      fingerPrint = true;
//      
//    }
//    else
//    {
//      //Serial.println("Finger not found");
//      fingerPrint = false;
//    }
//  }
//  else
//  {
//    //Serial.println("Please press finger");
//    fingerPrint = false;
//  }
//  delay(100);
//  
//  return fingerPrint;
//}
//
//void Enroll()
//{
//    // Enroll test
//
//    // find open enroll id
//    int enrollid = 0;
//    bool usedid = true;
//    while (usedid == true)
//    {
//        usedid = fps.CheckEnrolled(enrollid);
//        if (usedid==true) enrollid++;
//    }
//    fps.EnrollStart(enrollid);
//
//    // enroll
//    //Serial.print("Press finger to Enroll #");
//    //Serial.println(enrollid);
//    while(fps.IsPressFinger() == false) delay(100);
//    bool bret = fps.CaptureFinger(true);
//    int iret = 0;
//    if (bret != false)
//    {
//        //Serial.println("Remove finger");
//        fps.Enroll1();
//        while(fps.IsPressFinger() == true) delay(100);
//        //Serial.println("Press same finger again");
//        while(fps.IsPressFinger() == false) delay(100);
//        bret = fps.CaptureFinger(true);
//        if (bret != false)
//        {
//            //Serial.println("Remove finger");
//            fps.Enroll2();
//            while(fps.IsPressFinger() == true) delay(100);
//            //Serial.println("Press same finger yet again");
//            while(fps.IsPressFinger() == false) delay(100);
//            bret = fps.CaptureFinger(true);
//            if (bret != false)
//            {
//                //Serial.println("Remove finger");
//                iret = fps.Enroll3();
//                if (iret == 0)
//                {
//                    //Serial.println("Enrolling Successfull");
//                }
//                else
//                {
//                    //Serial.print("Enrolling Failed with error code:");
//                    //Serial.println(iret);
//                
//                }
//            }
//            else; //Serial.println("Failed to capture third finger");
//        }
//        else; //Serial.println("Failed to capture second finger");
//    }
//    else; //Serial.println("Failed to capture first finger");
//}

//void blinkyFPS()
//{
//  while(1){
//   // FPS Blink LED Test
//    fps.SetLED(true); // turn on the LED inside the fps
//  delay(1000);
//  fps.SetLED(false);// turn off the LED inside the fps
//  delay(1000);
//  }
//}



