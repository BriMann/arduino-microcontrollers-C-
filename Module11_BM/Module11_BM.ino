/*
Module 11 lab, Voltage Indicator
Brian Mann
April 17, 2026
This sketch will activate an LED bar graph module to visually showcase the charge and discharge of an RC circuit.
*/

/*
 * -----------------------------------------------------------------------------
 * GEMINI AI ASSISTANCE & USE DISCLAIMER
 * -----------------------------------------------------------------------------
 * Date: April 17, 2026
 * * This code was developed with the assistance of Gemini, an AI model by Google.
 * * DISCLAIMER: This code is provided "as is" for educational purposes. While 
 * designed to meet the user's specifications, it should be tested in a 
 * controlled environment. The user assumes all responsibility for hardware 
 * safety, including the use of proper current-limiting resistors and capacitor 
 * polarity to prevent component failure or injury.

 * * AI-GENERATED COMPONENTS:
 * 1. PIN DEFINITIONS: The array-based mapping of digital pins 2-11. (Lines 31-32)

 * 2. RC LOGIC: The implementation of analogRead() to interpret the RC time 
 * constant curve. (Line 48)

 * 3. MAPPING LOGIC: The use of the map() function to translate voltage levels 
 * into discrete LED segments. (Line 51)

 * 4. CONTROL LOOP: The 'for' loop logic used to dynamically update the state 
 * of the 10-segment bar graph. (Lines 54-60)
 * -----------------------------------------------------------------------------
 */

// Define the pins for the LED bar graph
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; 
const int analogPin = A0; // Connect to the junction of the Resistor and Capacitor

void setup() {
  // Initialize pins 2-11 as OUTPUT
  for (int i = 0; i < 10; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Read voltage from the RC circuit junction
  int sensorValue = analogRead(analogPin);
  
  // Map the analog value (0-1023) to the number of LEDs (0-10)
  // We use a slight offset at the top (1000 instead of 1023) to ensure 
  // all LEDs light up even if there's a tiny voltage drop.
  int ledLevel = map(sensorValue, 0, 1000, 0, 10);

  // Sequential LED control logic
  for (int i = 0; i < 10; i++) {
    // If the current LED index is less than the ledLevel, turn it on
    if (i < ledLevel) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }

  delay(10); // Small delay for ADC stabilization
}