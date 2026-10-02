/*
Module 8 lab, Stepper Motor
Brian Mann
March 13, 2026
This sketch will activate a stepper motor. When you press the button, the rotation direction of the stepper motor will reverse.
*/

/*
  Google Gemini was used to help with the implementation of the reverse button section of code.
  3/27/2026 Querey: "coding for arduino, have a button press reverse a stepper motor 28BYJ-48"
  Lines Influenced: 42-47
*/


#include <Stepper.h>
const int stepsPerRevolution = 2048;        // the number of steps per revolution the stepper motor can take
const int rolePerMinute = 16;               // Adjustable range of 28BYJ-48 stepper is 0~17 rpm
const int reverseMotorDirectionButton = 2;  // connects the reverse motor direction button to pin 2
int reverseButtonDetectionState = 0;
int currentReverseButtonState = 0;
int previousReverseButtonState = 0;

// initialize stepper object, sets how many steps it will take and sets the connection pins with MCU
Stepper stepper(stepsPerRevolution, 3, 4, 5, 6);

void setup() {
  pinMode(reverseMotorDirectionButton, INPUT);
  stepper.setSpeed(rolePerMinute); // Sets the rotation speed of the stepper motor
}

void loop() {
  //Lines 33-40 Check if the Reverse Button was pressed, then save its current and previous button state.
  currentReverseButtonState = digitalRead(reverseMotorDirectionButton);
  if (currentReverseButtonState != previousReverseButtonState) {
    if (currentReverseButtonState == HIGH) {
      reverseButtonDetectionState = (reverseButtonDetectionState + 1) % 2;
    }
    delay(50);  // Debounce
  }
  previousReverseButtonState = currentReverseButtonState;

  //Lines 43-47 Reverse the rotation direction of the Stepper Motor if the Reverse Button was pressed, otherwise the Stepper Motor will rotate forward
  if (reverseButtonDetectionState == HIGH) {
    stepper.step(-rolePerMinute);  //Reverse the rotation direction
  } else {
    stepper.step(rolePerMinute);  //Rotates the stepper motor
  }
}
