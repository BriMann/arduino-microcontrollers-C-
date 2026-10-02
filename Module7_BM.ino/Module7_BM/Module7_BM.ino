/*
Module 8 lab, Stepper Motor
Brian Mann
March 13, 2026
This sketch will activate a stepper motor. When you press the button, the rotation direction of the stepper motor will reverse.
*/
#include "SevSeg.h"
SevSeg Briansevseg; //initialize a SevSeg object

#define ThermistorPin A0  //the thermistor attach to const int analogPin = A0;
const float R1 = 10000;   //nominal value of pull-up resitor
float logR2, R2;          //R2 is the calibrated thermistor resistance at 25C
const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
  byte numDigits = 4;  //Define the number of digits the Seven Segment will display
  //Connects digit pins of 4 digit display {d1, d2, d3, d4} to board pins {13, 12, 11, 10} in that order
  byte digitPins[] = { 13, 12, 11, 10 };
  //Connects segment pins of 4 digit display {a, b, c, d, e, f, g, dp} to board pins {2, 3, 4, 5, 6, 7, 8, 9} in that order
  byte segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
  //delcares that resistor is put on the pin segment, not on the cathode
  bool resistorsOnSegments = true;
  //defines the configuration of the seven segment display hardware as a common cathode
  byte hardwareConfig = COMMON_CATHODE;
  // initialize the 4 digit seven segment display
  Briansevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
}

void loop() {
  //read thermistor value
  long ThermistorValue = analogRead(ThermistorPin);
  //the Steinhart-Hart equation for calculating the temperature of a thermistor
  R2 = R1 * (1023.0 / ThermistorValue - 1.0);
  logR2 = log(R2);
  float TempK = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  float TempC = TempK - 273.15;
  float TempF = (TempC * 9.0) / 5.0 + 32.0;

  static unsigned long timer = millis();

  //displays the thermistor temperature in Farenheit
  if (millis() >= timer) {
    timer += 300;
    Briansevseg.setNumber(TempC, 2);
  }
  //clear the 7 segment display for the next temperature reading
  Briansevseg.refreshDisplay();
}
