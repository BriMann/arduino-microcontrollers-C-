
/*
 * =================================================================
 * PROJECT: Arduino Mega 2560 Musical Workstation
 * DATE: May 1, 2026
 * AUTHOR: Brian Mann
 *
 * SETUP:
 * 1. Install "SevSeg" and "LiquidCrystal I2C" libraries.
 * 2. Wiring: 7 Bank buttons (D22-28), Joystick (A0 & D48), 
 * Mode Switch (D50), Select Button (D49), Rest Button(D47), Buzzer (D11).
 * 3. 3461AS-1 Display: Digits on D30-33, Segments on D34-41.
 *
 * AI ATTRIBUTION:
 * This program was developed in collaboration with Google Gemini (AI).
 * AI contributions include:
 * - Structured frequency/duration storage (Structs)
 * - Rhythmic snapping logic (Quantization)
 * - Error handling for SevSeg library type-mismatch
 * - Integrated Rest/Silence recording logic
 * =================================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SevSeg.h>


// --- PIN DEFINITIONS ---
#define JOY_X A0
#define JOY_Z 48        // Joystick Button click
#define SELECT_BTN 49   // Used for saving and clearing
#define MODE_SWITCH 50  // Left (Selection) / Right (Recording)
#define REST_BTN 47     // Rest button
#define BUZZER 11       // Piezo/Passive buzzer


// Buttons for the 7 individual note banks
int bankPins[7] = { 22, 23, 24, 25, 26, 27, 28 };


// --- MUSICAL TIMING (BPM 120) ---
const unsigned long WHOLE_MS = 2000;
const unsigned long HALF_MS = 1000;
const unsigned long QUART_MS = 500;
const unsigned long EIGHT_MS = 250;
const unsigned long SIXT_MS = 125;
const unsigned long THIRT_MS = 62;


// --- DATA STRUCTURES ---
struct Note {  // Defines what is contained in an instance of a note
  unsigned int frequency;
  const char* name;
};


struct RecordedNote {  // Defines what is contained in an instance of a recorded note
  unsigned int pitch;
  unsigned long durationMs;
  const char* durationName;
};


// --- NOTE DATA ARRAYS (Octaves 0 through 8) ---
Note noteA[9] = { { 28, "A0" }, { 55, "A1" }, { 110, "A2" }, { 220, "A3" }, { 440, "A4" }, { 880, "A5" }, { 1760, "A6" }, { 3520, "A7" }, { 7040, "A8" } };
Note noteB[9] = { { 31, "B0" }, { 62, "B1" }, { 123, "B2" }, { 247, "B3" }, { 494, "B4" }, { 988, "B5" }, { 1976, "B6" }, { 3951, "B7" }, { 7902, "B8" } };
Note noteC[9] = { { 16, "C0" }, { 33, "C1" }, { 65, "C2" }, { 131, "C3" }, { 262, "C4" }, { 523, "C5" }, { 1047, "C6" }, { 2093, "C7" }, { 4186, "C8" } };
Note noteD[9] = { { 18, "D0" }, { 37, "D1" }, { 73, "D2" }, { 147, "D3" }, { 294, "D4" }, { 587, "D5" }, { 1175, "D6" }, { 2349, "D7" }, { 4699, "D8" } };
Note noteE[9] = { { 21, "E0" }, { 41, "E1" }, { 82, "E2" }, { 165, "E3" }, { 330, "E4" }, { 659, "E5" }, { 1319, "E6" }, { 2637, "E7" }, { 5274, "E8" } };
Note noteF[9] = { { 22, "F0" }, { 44, "F1" }, { 87, "F2" }, { 175, "F3" }, { 349, "F4" }, { 698, "F5" }, { 1397, "F6" }, { 2794, "F7" }, { 5588, "F8" } };
Note noteG[9] = { { 25, "G0" }, { 49, "G1" }, { 98, "G2" }, { 196, "G3" }, { 392, "G4" }, { 784, "G5" }, { 1568, "G6" }, { 3136, "G7" }, { 6272, "G8" } };


const unsigned int REST_FREQ = 0;  // Defining a rest as 0Hz


// Pointer array links button index (0-6) to the note arrays above
Note* allNotes[7] = { noteA, noteB, noteC, noteD, noteE, noteF, noteG };


// --- SYSTEM STATE ---
unsigned int savedPitches[7] = { 440, 494, 523, 587, 659, 698, 784 };
RecordedNote yourSong[100];                          // Stores up to 100 notes
int activeBank = 0, octaveIndex = 4, songIndex = 0;  // Keeps track of when the song you recorded starts and ends, so it plays the correct notes and note durations.
bool joyReady = true;                                // Keeps track of when the joystick is flicked left or right.


// --- HARDWARE OBJECTS ---
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize an LCD Object
SevSeg sevseg;                       // Initialize a SevSeg Object


void setup() {
  // 1. Setup 4-digit 7-segment (3461AS-1 is Common Cathode)
  byte numDigits = 4;
  byte digitPins[] = { 30, 31, 32, 33 };                    // Digit cathodes
  byte segmentPins[] = { 34, 35, 36, 37, 38, 39, 40, 41 };  // Segments A-DP
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, true);
  sevseg.setBrightness(90);

  // 2. Setup LCD
  lcd.init();
  lcd.backlight();

  // 3. Setup Buttons & Inputs
  for (int i = 0; i < 7; i++) pinMode(bankPins[i], INPUT_PULLUP);  // Sets up all seven note buttons.
  pinMode(SELECT_BTN, INPUT_PULLUP);
  pinMode(MODE_SWITCH, INPUT_PULLUP);
  pinMode(JOY_Z, INPUT_PULLUP);
  pinMode(REST_BTN, INPUT_PULLUP);

  updateDisplays();
}


void loop() {
  // CRITICAL: briansevseg.refreshDisplay() must be called every single loop
  // to prevent flickering on the 3461AS-1 display.
  sevseg.refreshDisplay();

  // Handle Playback: Priority check for joystick button
  if (digitalRead(JOY_Z) == LOW) {
    playStoredSong();
  }

  // Handle Mode-Specific Logic
  bool isRecordingMode = (digitalRead(MODE_SWITCH) == LOW);
  if (isRecordingMode) {
    handleClearFunction();  // Check for long-press clear
    handleRestRecording();  // Check for Rest Button
    runRecordingMode();
  } else {
    runSelectionMode();
  }
}


/**
 * SELECTION MODE: 
 * Navigate the octave arrays with the joystick and save them to banks.
 */
void runSelectionMode() {
  for (int i = 0; i < 7; i++) {
    if (digitalRead(bankPins[i]) == LOW) {
      activeBank = i;
      tone(BUZZER, allNotes[activeBank][octaveIndex].frequency, 150);
      updateDisplays();
      delay(200);  // Debounce
    }
  }

  // Joystick navigation (Left/Right) for Octaves 0-8
  int xVal = analogRead(JOY_X);
  if (abs(xVal - 512) > 300 && joyReady) {  // Checks if you have reached the end of the array list in either direction
    octaveIndex = (xVal > 800) ? min(octaveIndex + 1, 8) : max(octaveIndex - 1, 0);
    joyReady = false;
    tone(BUZZER, allNotes[activeBank][octaveIndex].frequency, 100);  // Plays the frequency as you scroll through the array.
    updateDisplays();
  }
  if (abs(xVal - 512) < 100) joyReady = true;  // Checks if joystick is set back to center position before reading next joystick flick, otherwise it scrolls through the array really fast

  // Save the currently browsed note into the button's pitch bank
  if (digitalRead(SELECT_BTN) == LOW) {
    savedPitches[activeBank] = allNotes[activeBank][octaveIndex].frequency;
    lcd.setCursor(0, 1);
    lcd.print("   **SAVED** ");  // Flash a temporary message
    delay(300);                  // So the "   **SAVED** " message is displayed long enough for the user to see it on the LCD display
    updateDisplays();            // Refresh to show the new saved note name
  }
}


/**
 * RECORDING MODE: 
 * Captures how long a button is held and snaps it to musical timing.
 */
void runRecordingMode() {
  for (int i = 0; i < 7; i++) {
    if (digitalRead(bankPins[i]) == LOW) {
      unsigned long startTime = millis();
      unsigned int pitch = savedPitches[i];
      tone(BUZZER, pitch);

      // Hold note until button is released
      while (digitalRead(bankPins[i]) == LOW) {
        sevseg.refreshDisplay();  // Refresh display while waiting
      }
      noTone(BUZZER);  // buzzer stops playing after button is released

      unsigned long heldTime = millis() - startTime;
      if (songIndex < 100) {
        yourSong[songIndex].pitch = pitch;                                                   // Saves the pitches of each note button you pressed
        yourSong[songIndex].durationMs = snapDuration(heldTime);                             // Standardize the time
        yourSong[songIndex].durationName = getDurationName(yourSong[songIndex].durationMs);  // Saves the names of each note button you pressed
        songIndex++;
      }
      updateDisplays();
      delay(150);  // Debounce
    }
  }
}


/**
 * CLEAR FUNCTION: 
 * Hold SELECT for 2 seconds while in recording mode to reset the song.
 */
void handleClearFunction() {
  if (digitalRead(SELECT_BTN) == LOW) {
    unsigned long holdStart = millis();
    while (digitalRead(SELECT_BTN) == LOW) {
      sevseg.refreshDisplay();
      if (millis() - holdStart >= 2000) {  // 2 second threshold
        songIndex = 0;
        lcd.clear();
        lcd.print("MEMORY WIPED!");
        tone(BUZZER, 100, 500);
        delay(1000);  // So the "Memory Wiped!" message is displayed long enough for the user to see it on the LCD display
        updateDisplays();
        return;
      }
    }
  }
}


/**
 * RECORDS A REST (SILENCE)
 */
void handleRestRecording() {
  if (digitalRead(REST_BTN) == LOW) {
    unsigned long startTime = millis();

    // Immediate Visual Feedback
    lcd.clear();
    lcd.print("RECORDING...");
    lcd.setCursor(0, 1);
    lcd.print("TYPE: REST");

    // While button is held, play silence and show "REST" on 7-segment
    while (digitalRead(REST_BTN) == LOW) {
      sevseg.setChars("REST");
      sevseg.refreshDisplay();
      noTone(BUZZER);  // Ensure no sound is playing
    }

    unsigned long heldTime = millis() - startTime;

    // Save to song array
    if (songIndex < 100) {
      yourSong[songIndex].pitch = REST_FREQ;  // Store as 0
      yourSong[songIndex].durationMs = snapDuration(heldTime);
      yourSong[songIndex].durationName = getDurationName(yourSong[songIndex].durationMs);
      songIndex++;
    }

    updateDisplays();
    delay(150);  // Debounce
  }
}


/**
 * PLAYBACK LOOP: 
 * Cycles through the song array while the joystick button is held.
 */
void playStoredSong() {
  if (songIndex == 0) return;

  while (digitalRead(JOY_Z) == LOW) {
    for (int i = 0; i < songIndex; i++) {
      if (digitalRead(JOY_Z) == HIGH) break;

      // Check if this note is a REST
      if (yourSong[i].pitch == REST_FREQ) {
        lcd.setCursor(0, 1);
        lcd.print("PLAYING: REST   ");
        sevseg.setChars("REST");
        noTone(BUZZER);  // Keep the buzzer quiet
      } else {           // Displays which notes are playing during the song
        lcd.setCursor(0, 1);
        lcd.print("PLAYING: ");
        lcd.print(yourSong[i].pitch);
        lcd.print("Hz  ");
        sevseg.setNumber((int)yourSong[i].pitch, -1, false);
        tone(BUZZER, yourSong[i].pitch);
      }

      // Keep refreshing the 7-segment for the duration of the note/rest
      unsigned long pStart = millis();
      while (millis() - pStart < yourSong[i].durationMs) {
        sevseg.refreshDisplay();
      }

      noTone(BUZZER);

      // Brief gap so consecutive same-notes don't blend
      unsigned long gapStart = millis();
      while (millis() - gapStart < 30) {
        sevseg.refreshDisplay();
      }
    }
  }
  updateDisplays();
}

/**
 * HELPER: Snaps raw millisecond hold time to musical durations.
 */
unsigned long snapDuration(unsigned long ms) {
  if (ms >= 1500) return WHOLE_MS;
  if (ms >= 750) return HALF_MS;
  if (ms >= 375) return QUART_MS;
  if (ms >= 180) return EIGHT_MS;
  if (ms >= 90) return SIXT_MS;
  return THIRT_MS;
}

const char* getDurationName(unsigned long ms) {
  if (ms == WHOLE_MS) return "Whole";
  if (ms == HALF_MS) return "Half";
  if (ms == QUART_MS) return "Quarter";
  if (ms == EIGHT_MS) return "8th";
  if (ms == SIXT_MS) return "16th";
  return "32nd";
}

/**
 * REFRESH LCD:
 * Updates text based on current bank, note name, and mode.
 */
void updateDisplays() {
  bool isRec = (digitalRead(MODE_SWITCH) == LOW);
  lcd.clear();

  if (isRec) {
    // --- RECORDING MODE VIEW ---
    lcd.setCursor(0, 0);
    lcd.print("REC MODE");
    lcd.setCursor(0, 1);
    lcd.print("Notes Count: ");
    lcd.print(songIndex);
    sevseg.setNumber((int)songIndex, -1);
  } else {
    // --- SELECTION / EDIT MODE VIEW ---
    Note currentBrowsed = allNotes[activeBank][octaveIndex];

    // Line 1: What you are currently looking at with the joystick
    lcd.setCursor(0, 0);
    lcd.print("BROWSE: ");
    lcd.print(currentBrowsed.name);

    // Line 2: What is actually saved to this button (Bank)
    lcd.setCursor(0, 1);
    lcd.print("SAVED: ");

    // We search the current bank's array to find the name tied to the saved frequency
    bool found = false;
    for (int i = 0; i < 9; i++) {
      if (allNotes[activeBank][i].frequency == savedPitches[activeBank]) {
        lcd.print(allNotes[activeBank][i].name);
        found = true;
        break;
      }
    }
    if (!found) lcd.print("None");

    // Update 7-segment with the frequency you are currently browsing
    sevseg.setNumber((int)currentBrowsed.frequency, -1);
  }
}