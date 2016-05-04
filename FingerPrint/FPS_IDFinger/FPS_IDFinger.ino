/* 
	FPS_Enroll.ino - Library example for controlling the GT-511C3 Finger Print Scanner (FPS)
	Created by Josh Hawley, July 23rd 2013
	Licensed for non-commercial use, must include this license message
	basically, Feel free to hack away at it, but just give me credit for my work =)
	TLDR; Wil Wheaton's Law

	This sketch will attempt to identify a previously enrolled fingerprint.
*/

#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

// Hardware setup - FPS connected to:
//	  digital pin 4(arduino rx, fps tx)
//	  digital pin 5(arduino tx - 560ohm resistor fps tx - 1000ohm resistor - ground)
//		this brings the 5v tx line down to about 3.2v so we dont fry our fps

FPS_GT511C3 fps(3,2);
//FPS_GT511C3 fps(13, 14);

void setup()
{
	Serial.begin(9600);
 Serial.println("WTF");
	delay(100);
  Serial.println("open");
   fps.UseSerialDebug = true;
	fps.Open();
 Serial.println("SET LED");
	fps.SetLED(true);

 Enroll();
}

void loop()
{

	// Identify fingerprint test
	if (fps.IsPressFinger())
	{
		fps.CaptureFinger(false);
		int id = fps.Identify1_N();
		if (id <200)
		{
			Serial.print("Verified ID:");
			Serial.println(id);
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

