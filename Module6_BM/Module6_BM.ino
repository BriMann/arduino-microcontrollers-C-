/*
Module 6 lab, Joystick and LED Controls
Brian Mann
March 6, 2026
This sketch will perform two seperate functions dependant on the inputs of the Joystick Module:
  When the Joystick button is pressed down, upon release the LED will turn ON. Press and release
  the button again to turn OFF.
  When the LED is ON and you move the Joystick module around (left to right, up and down), the 
  brightness of the LED will change.
*/

/*
  Google Gemini was used to help with the implementation of the brightness adjusting section of code.
  3/6/2026 Querey: "coding for arduino, have a joystick adjust the brightness of an led and click the 
  joystick to turn the led on/off"
  Lines Influenced: 55-66
*/

const int swPin = 8;          // Joystick Z-axis pinSW (when you click the joystick) connected to pin 8
const int joystickXPin = A0;  // Joystick X-axis pinVRX (left to right) connected to pin A0
const int joystickYPin = A1;  // Joystick Y-axis pinVRY (up to down) connected to pin A1
const int ledPin = 9;         // Red LED connected to pin 9
const int delayValue = 50;
int detectionState = LOW;
int LastButtonState = HIGH;
int ButtonState = HIGH; /*Button states on lines 24-25 are both set to HIGH
                          to ensure the LED will start out in the OFF state when
                          the code is uploaded to the arduino*/

void setup() {
  pinMode(swPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(swPin, HIGH);
  Serial.begin(9600);
}

void loop() {
  delay(delayValue);
  Serial.print("X: ");
  delay(delayValue);
  Serial.print(analogRead(joystickXPin), DEC);  // print the value of VRX in DEC
  delay(delayValue);
  Serial.print("|Y: ");
  Serial.print(analogRead(joystickYPin), DEC);  // print the value of VRX in DEC
  delay(delayValue);
  Serial.print("|Z: ");
  delay(delayValue);
  Serial.println(digitalRead(swPin));  // print the value of SW
  delay(delayValue);  // Debounce
  ButtonState = digitalRead(swPin);
  if (ButtonState != LastButtonState) {
    if (ButtonState == HIGH) {
      detectionState = (detectionState + 1) % 2;  // LED will only turn ON after the Joystick button is released
      Serial.print("The detection state is:");
      Serial.println(detectionState);
      digitalWrite(ledPin, detectionState);
    }
    delay(delayValue);  //Second Debounce to prevent any noticeable light flickering
  }
  LastButtonState = ButtonState;

  //Reads Joystick Y-axis to adjust brightness of LED
  if (digitalRead(ledPin) == HIGH) {
    int SensorValue = analogRead(joystickYPin);
    /*Map Joystick Y-axis (0-1023) to LED brightness value (0,255).
    The 4th argument in the Map Function is set to 5 instead of 0, so
    the joystick doesn't turn off the LED if it's moved downward. Otherwise 
    the light turns off and you would need to click the joystick again*/
    int Brightness = map(SensorValue, 0, 1023, 5, 255);
    analogWrite(ledPin, Brightness);
  } 
  else {
    analogWrite(ledPin, 0);
  }
}
