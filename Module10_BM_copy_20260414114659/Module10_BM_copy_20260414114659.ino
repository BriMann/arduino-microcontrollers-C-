#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int echoPin = 4;
const int trigPin = 5;

const int redLED = 6;
const int yellowLED = 7;
const int greenLED = 8;

const int passiveBuzzerPin = 9;

bool alertState = false;

const unsigned long safeAlert = 1000;
const unsigned long safeLCD = 700;

const unsigned long cautionAlert = 800;
const unsigned long cautionLCD = 400;

const unsigned long warningAlert = 300;
const unsigned long warningLCD = 200;

unsigned long previousAlertMillis = 0;  //saves the previous time in milliseconds
unsigned long previousLCDMillis = 0;

unsigned long alertInterval = safeAlert;
unsigned long lcdInterval = safeLCD;

LiquidCrystal_I2C lcd(0x27, 16, 2);  //initialize LCD Object

void setup() {
  Serial.begin(9600);
  // initialize the LCD
  lcd.init();
  // turn on the LCD backlight
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
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH, 30000);
  Serial.println(duration);
  delay(50);
  if (duration == 0){
    return -1;
  }
  return duration * 0.034 / 2;
}

void updateAlarm(unsigned long alarmDistance) {
  if (alarmDistance > 30) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
    noTone(passiveBuzzerPin);

  } else if (alarmDistance >= 15 && alarmDistance < 30) {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, alertState);
    digitalWrite(redLED, LOW);
    if (alertState) {
      tone(passiveBuzzerPin, 1000);
    } else {
      noTone(passiveBuzzerPin);
    }

  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, alertState);
    if (alertState) {
      tone(passiveBuzzerPin, 2000);
    } else {
      noTone(passiveBuzzerPin);
    }
  }
}

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


  if (currentMillis - previousAlertMillis >= alertInterval) {
    previousAlertMillis = currentMillis;
    alertState = !alertState;
    updateAlarm(distance);
  }


  if (currentMillis - previousLCDMillis >= lcdInterval) {
    previousLCDMillis = currentMillis;
    updateLCD(distance);
  }
}