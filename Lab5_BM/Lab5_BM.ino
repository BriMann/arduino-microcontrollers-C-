/*
Module 5 lab, Button Controlled Buzzers
Brian Mann
February 20, 2026
This sketch will perform three seperate functions dependant on three different buttons:
  When the first button is pressed, the active buzzer will emit a sound.
  When the second button is pressed, the passive buzzer will emit the sound of a british siren.
  When the third button is pressed, the passive buzzer will emit the sound of the Hawaii 5-0 theme song.
*/
#include "pitches.h"
const int ActiveBuzzerButtonPin = 11;  // Active Buzzer input pin connected to pin 11
const int SirenBuzzerPin = 3;          // Siren Buzzer output pin connected to pin 11
const int ThemeSongBuzzerPin = 4;      // Theme Song Buzzer output pin connected to pin 11
const int SirenButtonPin = 5;          // Siren Button input pin connected to pin 11
const int ThemeSongButtonPin = 6;      // Theme Song Buzzer input pin connected to pin 11

const int ThemeSong[] = {  // Initialize notes pitchs for Theme Song melody
  NOTE_E4, NOTE_E4, NOTE_G4,
  NOTE_B4, NOTE_A4, NOTE_E4,
  REST, NOTE_E4, NOTE_E4,
  NOTE_D4, NOTE_G4, NOTE_E4,
  REST
};

const int ThemeSongNoteDurations[] = {  // Initialize note duration for Theme Song melody
  8, 8, 4,
  4, 2, 2,
  4, 8, 8,
  4, 4, 1,
  4
};

const int Siren[] = {  // Initialize notes pitchs for Siren melody
  NOTE_FS4, NOTE_C4,
  NOTE_FS4, NOTE_C4,
  NOTE_FS4, NOTE_C4,
  NOTE_FS4, NOTE_C4
};

const int SirenNoteDurations[] = {  // Initialize note duration for Siren melody
  2, 2, 2,
  2, 2, 2,
  2, 2
};

void setup() {  // Initialize serial for monitoring pot values
  pinMode(ActiveBuzzerButtonPin, INPUT);
  pinMode(SirenButtonPin, INPUT);
  pinMode(ThemeSongButtonPin, INPUT);
  pinMode(SirenBuzzerPin, OUTPUT);
  pinMode(ThemeSongBuzzerPin, OUTPUT);
}

void loop() {
  digitalWrite(ActiveBuzzerButtonPin, HIGH);
  int SirenButtonState = digitalRead(SirenButtonPin);
  int ThemeSongButtonState = digitalRead(ThemeSongButtonPin);

  while (SirenButtonState == 1) {  // Checks if the Siren Melody Button is pressed
    for (int thisNote = 0; thisNote < 8; thisNote++) {
      /*to calculate the note duration, take one second divided by the note type.
        e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.*/
      int noteDuration = 1000 / SirenNoteDurations[thisNote];
      tone(SirenBuzzerPin, Siren[thisNote], noteDuration);  // to distinguish the notes, set a minimum time between them.
      int pauseBetweenNotes = noteDuration * 1.00;
    }
    SirenButtonState = digitalRead(SirenButtonPin);
  }

  while (ThemeSongButtonState == 1) {  // Checks if the Theme Song Melody Button is pressed
    for (int thatNote = 0; thatNote < 13; thatNote++) {
      /*Lines 72-74 Uses the same code as the Siren Button while loop 
        thus see loop comments from lines 61-62 to understand functionality of code*/
      int noteDuration = 1000 / ThemeSongNoteDurations[thatNote];
      tone(ThemeSongBuzzerPin, ThemeSong[thatNote], noteDuration);  // to distinguish the notes, set a minimum time between them.
      int pauseBetweenNotes = noteDuration * 1.30;                  // the note's duration + 30% seems to work well:
      delay(pauseBetweenNotes);
    }
    ThemeSongButtonState = digitalRead(ThemeSongButtonPin);
  }
}
