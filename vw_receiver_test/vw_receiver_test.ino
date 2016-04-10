#include <Average.h>

//simple Tx on pin D12
//Written By : Mohannad Rawashdeh
// 3:00pm , 13/6/2013
void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    //noTone(10);
    //digitalWrite(10,0);
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
        int i;
    int sampleSize = 20;
    Average<int> ave(25);
    int thresholdValue;
    
        if(buf[0]=='1'){
          digitalWrite(13,1);
          Serial.print("Received:");
          Serial.println(char(buf[0]));
          soundZeAlarm();
          
         }  
        else if(buf[0]=='0'){
          Serial.print("Received:");
          Serial.println(char(buf[0]));
          digitalWrite(13,0);
         }
         

        }//end if
        //read data in from RF sampleSize times and store in samples[]
        //for(int i=0;i < sampleSize;i++)
        //{

           //Serial.print("in sample");
          
          //Listen to RF, collect sample data
          if (vw_get_message(buf, &buflen)) // Non-blocking
          {
            
              Serial.println("Got: ");
              Serial.println(buf[0]);
              //add sample to sample array
              //samples[i] = buf[0];  //**********MAY NEED SOME CONVERTING INTO DECIMAL NUMBER FROM BUFFER
              //ave.push(buf[0]);
              //Serial.print("Received and Pushed: ");
              //Serial.print(buf[0]);
              
              
          }//end if

       // }//end for

        //take average of array values
        //int pressureAvg = ave.mean();

        // set threshold to -30%??  of average value?
       // thresholdValue = pressureAvg - pressureAvg*(0.3);
        //Serial.print("Threshold Set To: ");
        //Serial.println(thresholdValue);


}



void soundZeAlarm()
{

// iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 78; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(10, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(10);
  }
   
}

