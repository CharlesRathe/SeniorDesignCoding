 /***************** 
  *  Standalone Tiny45
  *  Speaker IC
  *****************/
  
 #include "pitches.h"

// Song Variables
const int alarmLength = 28;   // Length of two pitch alarm
const int alarmPin = 0;                // Defines PIN which alarm is sent to
const int inputPin = 1;

const int alarm[] = {NOTE_F7, 0, NOTE_C7, 0, NOTE_F7, 0, NOTE_C7, 0, 
NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,
NOTE_F7, 0, NOTE_C7, 0,NOTE_F7, 0, NOTE_C7, 0,};
 
// Holds the notes of the alarm
 const int alarmDurations[] = {
12, 12, 12, 12,
12, 12, 12, 12,
12, 12, 12, 12,
12, 12, 12, 12,
12, 12, 12, 12,
12, 12, 12, 12,
12, 12, 12, 12,
};

void setup() {
  // Set pin I/O
  pinMode(alarmPin, OUTPUT);
  pinMode(inputPin, INPUT);

}

void loop() {
  

    for (int thisNote = 0; thisNote < alarmLength; thisNote++) {

      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1400 / alarmDurations[thisNote];
      tone(alarmPin, alarm[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
  
      // stop the tone playing:
      noTone(alarmPin);
    }

}
