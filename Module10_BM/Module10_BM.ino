/*
Module 10 lab, Backup Warning System
Brian Mann
April 16, 2026
This sketch will activate an Ultrasonic Sensor and perform 3 functions:
    Activate one of 3 colored LEDs
    Activate a passive buzzer
    Display the distance it measures on an LCD screen
  for distances less than 15 cm, the update must be 0.3 seconds, 
  the red LED blinks, and the buzzer beeps; 

  for distances < 30 cm but >15 cm, the update is 0.8 seconds, 
  the yellow LED blinks, and the buzzer beeps at a slower interval; 

  for distance >30 cm, the green LED is solid color (no blinking) and there is no buzzer warning sound. 
  The LCD should refresh and display the distance to the object for all three cases.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int echoPin = 4;  // Echo pin for ultrasonic sensor
const int trigPin = 5;  // Trigger pin for ultrasonic sensor

const int redLED = 6;     // Red LED for warning zone
const int yellowLED = 7;  // Yellow LED for caution zone
const int greenLED = 8;   // Green LED for safe zone

const int passiveBuzzerPin = 9;

bool alertState = false;  // Toggles blinking lights and buzzer alarm

const unsigned long safeAlert = 1000;  // Safe alert interval
const unsigned long safeLCD = 700;     // Safe LCD refresh interval

const unsigned long cautionAlert = 800;  // Caution alert interval
const unsigned long cautionLCD = 400;    // Caution LCD refresh interval

const unsigned long warningAlert = 300;  // Warning alert interval
const unsigned long warningLCD = 200;    // Warning LCD refresh interval

unsigned long previousAlertMillis = 0;  //Saves the previous alert time in milliseconds
unsigned long previousLCDMillis = 0;    // Saves the previous LCD refresh time in milliseconds

unsigned long alertInterval = safeAlert;  // Current alert interval
unsigned long lcdInterval = safeLCD;      // Current LCD refresh interval

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize LCD Object

/*
  Google Gemini was used to help with the implementation of the buzzer Alarm portions of the code.
  4/16/2026 Querey: "play an alarm sound on a passive buzzer"
  Lines Influenced: 108-112, 121-125
*/

/*
  Google Gemini was used to help with the implementation of the ultrasonic sensor portions of the code.
  4/16/2026 Querey: "arduino read distance in cm with ultrasonic sensor"
  Lines Influenced: 77-90
*/

void setup() {
  // Initialize the LCD
  lcd.init();
  // Turn on the LCD backlight
  lcd.backlight();
  lcd.begin(16, 2);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(passiveBuzzerPin, OUTPUT);
}

float readSensorData() {
  digitalWrite(trigPin, LOW);  // Clear trigger pin
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // Send 10 microsecond pulse
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH, 30000);  // Measure echo pulse
  delay(50);
  if (duration == 0) {  // Returns invalid if timeout occurs
    return -1;
  }
  return duration * 0.034 / 2;  // Convert pulse duration to distance in cm
}

void updateAlarm(unsigned long alarmDistance) {

  // Safe zone alarm
  if (alarmDistance > 30) {
    digitalWrite(greenLED, HIGH);  // Green LED is solid
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
    noTone(passiveBuzzerPin);  // No buzzer warning

  } 

  // Caution zone alarm
  else if (alarmDistance >= 15 && alarmDistance < 30) {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, alertState);  //Yellow LED blinks
    digitalWrite(redLED, LOW);
    if (alertState) {
      tone(passiveBuzzerPin, 1000);  // Slower warning tone
    } else {
      noTone(passiveBuzzerPin);
    }

  } 
  
  // Warning zone alarm
  else {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, alertState);  // Red LED blinks
    if (alertState) {
      tone(passiveBuzzerPin, 2000);  // Faster warning tone
    } else {
      noTone(passiveBuzzerPin);
    }
  }
}

// Displays measured distance from ultrasonic sensor
void updateLCD(unsigned long lcdDistance) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(lcdDistance);
  lcd.print(" cm");
}

void loop() {
  float distance = readSensorData();
  unsigned long currentMillis = millis();

  // Set alarm and LCD timing based on distance reading
  if (distance > 30) {
    alertInterval = safeAlert;
    lcdInterval = safeLCD;
  } else if (distance >= 15 && distance < 30) {
    alertInterval = cautionAlert;
    lcdInterval = cautionLCD;
  } else {
    alertInterval = warningAlert;
    lcdInterval = warningLCD;
  }

  // Update LED and buzzer alarm states using interval timing
  if (currentMillis - previousAlertMillis >= alertInterval) {
    previousAlertMillis = currentMillis;
    alertState = !alertState;
    updateAlarm(distance);
  }

  // Update LCD display using interval timing
  if (currentMillis - previousLCDMillis >= lcdInterval) {
    previousLCDMillis = currentMillis;
    updateLCD(distance);
  }
}