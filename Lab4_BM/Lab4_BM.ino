/*
Module 4 lab, Part 1: LED Matrix, Part 2: LCD Display
Brian Mann
February 13, 2026
This sketch will perform two seperate functions
The first function is to loop through the LED Matrix to turn ON/OFF each dot, row, and column.
  Followed by displaying an image.
The second function is to display my name on the first row of an LCD screen,
  while on the second row contiously incrementing a number from 0 to 9, then reset back to zero.
*/
#include "LedControl.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
const int dataPin = 12;                                               // LED matrix data pin connected to pin 12
const int clockPin = 11;                                              // LED matrix clock pin connected to pin 11
const int csPin = 10;                                                 // LED matrix data pin connected to pin 10
const int num_of_Devices = 1;                                         // Number of devices connected to LED Matrix
LedControl lc = LedControl(dataPin, clockPin, csPin, num_of_Devices);
LiquidCrystal_I2C lcd(0x27, 16, 2);                                   // declare the LCD with I2C address 0x27, 16 colunms and 2 rows.

void setup() {
  // shutdown command, if true shut down, if false power on
  lc.shutdown(0, false);
  // brightness of the display
  lc.setIntensity(0, 5);
  // clear whatever is currently on the display 
  lc.clearDisplay(0);
  // initialize the LCD
  lcd.init();
  // turn on the LCD backlight
  lcd.backlight();
  //Serial.begin(9600);
}

void loop() {
  lcd.clear();
  // set the starting point to upper left corner
  lcd.setCursor(0, 0);
  lcd.print("Brian Mann");
  // set the starting point to below the printed name
  lcd.setCursor(2, 1);
  for (int col = 0; col < 10; col++){ // Loop from 0 to 9 and print each number
    lcd.print(col);
    lcd.setCursor(2,1);
    delay(500);
    }
  delay(1000);
  // calling subroutines
  displayDot();
  displayCol();
  displayRow();
  displayPic();
}

void displayDot(){ // sequentially turns on each dot, starting in each row then going by column.
  lc.clearDisplay(0);
  delay(100);
  for (int row = 0; row < 8; row ++){
    for (int col = 0; col < 8; col ++){
      lc.setLed(0, row, col, true); // (address, row, column, boolean state)
        delay(50);
      }
  }
}

void displayCol(){ // turns on each column specified by the byte data, starting from the top row then working downwards
  lc.clearDisplay(0);
  delay(100);
  byte data = B01100110; // turns on the 2nd, 3rd, 6th, and 7th column
  for (int col = 0; col < 8; col++){
    lc.setColumn(0, col, data); // (address, column, boolean state)
    delay(100);
  }
}

void displayRow(){ // turns on each row specified by the byte data, starting from the left column then working towards the right
  lc.clearDisplay(0);
  delay(100);
  byte data = B10011001; // turns on the 1st, 4th, 5th, and 8th row
  for (int row = 0; row < 8; row++){
    lc.setRow(0, row, data); // (address, row, boolean state)
    delay(100);
  }
}

void displayPic(){ // displays an 8 bit picture based on the matrix value
  lc.clearDisplay(0);
  delay(100);
  byte pic[8] = { // declare an 8-bit picture, with 1's being ON, and 0's being OFF
    B11111111,
    B10111101,
    B10011001,
    B11111111,
    B11011011,
    B10000001,
    B11100111,
    B11111111};
  for(int col = 0; col < 8; col++)
    {lc.setColumn(0,col,pic[col]);}
}
