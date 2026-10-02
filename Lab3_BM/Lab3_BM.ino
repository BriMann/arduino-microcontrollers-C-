/*
Module 3 lab, Part 1: LED plus Potentiometer, Part 2: LED plus Button Switch
Brian Mann
Febuary 6, 2026
This sketch will perform two seperate functions
The first function is to read the input value of the potentiometer, map the value to 8-bit, 
  then write the value to the LED.
The second function is to read the voltage of a button_switch, then turn the LED ON or OFF
  based on the reading. 
*/
const int LEDpotentiometerPin = 9; // Sets the LED connected to the potentiometer to integer 9
const int LEDbuttonPin = 8;        // Sets the LED connected to the button to integer 8
const int ButtonPin = 2;           // Sets the button switch input to integer 2
const int PotSensorPin = A0;       // Sets the potentiometer input to ADC input pin A0
int buttonState = 0;               //buttonState is the boolean variable

void setup() {
  pinMode(LEDpotentiometerPin, OUTPUT);  // sets pin 9 as an output
  pinMode(LEDbuttonPin, OUTPUT);         // sets pin 8 as an output
  pinMode(ButtonPin, INPUT);             // sets pin 2 as an input
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(PotSensorPin);         // Reads the potentiomater voltage and saves the value
  int brightness = map(sensorValue, 0, 1024, 0, 255); // Map the potentiometer voltage reading to the LED brightness value (0-1024 is mapped to 0-255)
  analogWrite(LEDpotentiometerPin, brightness);       // Sets the LED to the value of the brightness variable
  delay(1000);
  Serial.print("Brightness Value");
  Serial.println(brightness);
  buttonState = digitalRead(ButtonPin);
  if (buttonState==HIGH){                             // If buttonState is High (not pressed), turn On the LED
    digitalWrite(LEDbuttonPin, HIGH);
    delay(1000);
    Serial.print("Button State");
    Serial.println(buttonState);
  }else{                                              // If buttonState is Low (pressed), turn Off the LED
    digitalWrite(LEDbuttonPin, LOW);
    delay(1000);
    Serial.print("Button State");
    Serial.println(buttonState);
  }
}
