////////////////////////////////////////////////
//    Reads in Force Sensor in analog port    //
//    If above a certain force, sends out     //
//    signal to transmitter                   //
////////////////////////////////////////////////

// Reads A0 every 100ms and sends voltage value over serial

void setup() 
{
  // Start serial at 9600 baud
  Serial.begin(9600); 

  pinMode(0, OUTPUT);
}

    void loop() 
{
  // Read the input on analog pin 0:
  int sensorValue = analogRead(A3);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  
  // Print out the value you read:
  if(voltage >= .5){
    // Send to rf
  }

  else{
    delay(500);
  }
  
  // Wait 100 milliseconds
  delay(100);
}
