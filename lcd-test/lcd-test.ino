 #include "LiquidCrystal.h" 
LiquidCrystal lcd(13, 11, A0, A1, A2, A3);


void setup() {
  // Set up LCD
  lcd.begin(16,2);            // Set LCD for 16 columns, 2 lines
  lcd.clear();    
  // Clear LCD and print intro
   lcd.setCursor(1,0);
  lcd.print("Hello ");
  lcd.setCursor(0,1);
  lcd.print("World! ");
  digitalWrite(10, LOW);
}

void loop() {
 

}
