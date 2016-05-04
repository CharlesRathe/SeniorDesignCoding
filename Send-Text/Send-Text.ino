 #include <SPI.h>
 #include <Ethernet.h>

#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

// Hardware setup - FPS connected to:
//    digital pin 4(arduino rx, fps tx)
//    digital pin 5(arduino tx - 560ohm resistor fps tx - 1000ohm resistor - ground)
//    this brings the 5v tx line down to about 3.2v so we dont fry our fps

FPS_GT511C3 fps(3,2);

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
  
void setup() {

  Serial.begin(9600);
  delay(100);
  Serial.println("Open FPS");
   fps.UseSerialDebug = true;
  fps.Open();
 Serial.println("SET LED");
 fps.SetLED(true);

 
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

 //send a text
 //textAlarm();

 Enroll();
}


void loop() {
  // Identify fingerprint test
  if (fps.IsPressFinger())
  {
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    if (id <200)
    {
      Serial.print("Verified ID:");
      Serial.println(id);
      textAlarm();
    }
    else
    {
      Serial.println("Finger not found");
    }
  }
  else
  {
    Serial.println("Please press finger");
  }
  delay(100);

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

void Enroll()
{
    // Enroll test

    // find open enroll id
    int enrollid = 0;
    bool usedid = true;
    while (usedid == true)
    {
        usedid = fps.CheckEnrolled(enrollid);
        if (usedid==true) enrollid++;
    }
    fps.EnrollStart(enrollid);

    // enroll
    Serial.print("Press finger to Enroll #");
    Serial.println(enrollid);
    while(fps.IsPressFinger() == false) delay(100);
    bool bret = fps.CaptureFinger(true);
    int iret = 0;
    if (bret != false)
    {
        Serial.println("Remove finger");
        fps.Enroll1();
        while(fps.IsPressFinger() == true) delay(100);
        Serial.println("Press same finger again");
        while(fps.IsPressFinger() == false) delay(100);
        bret = fps.CaptureFinger(true);
        if (bret != false)
        {
            Serial.println("Remove finger");
            fps.Enroll2();
            while(fps.IsPressFinger() == true) delay(100);
            Serial.println("Press same finger yet again");
            while(fps.IsPressFinger() == false) delay(100);
            bret = fps.CaptureFinger(true);
            if (bret != false)
            {
                Serial.println("Remove finger");
                iret = fps.Enroll3();
                if (iret == 0)
                {
                    Serial.println("Enrolling Successfull");
                }
                else
                {
                    Serial.print("Enrolling Failed with error code:");
                    Serial.println(iret);
                }
            }
            else Serial.println("Failed to capture third finger");
        }
        else Serial.println("Failed to capture second finger");
    }
    else Serial.println("Failed to capture first finger");
}

