/*
Module 9 lab, Environmental Sensor Station
Brian Mann
April 6, 2026
This sketch will activate three seperate Environmental Sensor Devices:
      A Water Sensor
      A Humidity and Temperature Sensor (DHT11)
      And a Light Intensity Sensor (Photoresistor)
  The LCD will start off on a bootup screen, prompting you to press the button.
  The first button press will display the light intensity measurement of the photoresistor
  The second button press will display the temperature and humidity measurements of the DHT11
  The third button press will display the whether any water is "detected" or "not detected" by the water sensor
  The fourth button press will take you back to the bootup screen
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#define DHT11_PIN 4
#define DHTTYPE DHT11              //define the type of DHT model used
const int sensorToggleButton = 2;  //Connect sensor toggle button input to pin 2
int detectionState = 0;
int buttonState = 0;
int lastButtonState = 0;
int numberOfButtonPresses = 0;
unsigned long previousMillis = 0;    //saves the previous time in milliseconds
LiquidCrystal_I2C lcd(0x27, 16, 2);  //initialize LCD Object
DHT dht(DHT11_PIN, DHTTYPE);         //initialize DHT Object

/*
  Google Gemini was used to help with the implementation of the DHT11 LCD print statement portions of the code.
  4/3/2026 Querey: "arduino lcd display print multiple things on the same line"
  Lines Influenced: 107-114
*/

/*
  Google Gemini was used to help with the implementation of the callMillis function.
  4/3/2026 Querey: "arduino how to properly use millis() to replace the delay function"
  Lines Influenced: 58-68
*/

/*
  Google Gemini was used to help with the initialization of a DHT11 object and a proper understanding of its library functions.
  4/3/2026 Querey: "arduino dht11 how to initialize"
  Lines Influenced: 18-19, 27, 105-106
*/

void setup() {
  // initialize the LCD
  lcd.init();
  // turn on the LCD backlight
  lcd.backlight();
  lcd.begin(16, 2);
  pinMode(sensorToggleButton, INPUT);
  dht.begin();
  Serial.begin(9600);
}

bool callMillis(unsigned long interval) {
  // Capture the current time
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    //if interval has passed, return true
    return true;
  }
  return false;
}
//When starting up the program, the LCD will prompt the user to press the button
void bootupScreen() {
  lcd.setCursor(0, 0);
  lcd.print("Press Btn");
}

//On the third button press, activate the water sensor module to detect moisture of surrounding area. then display on the second line of the LCD
void waterSensorModule() {
  // set the starting point to upper left corner
  lcd.setCursor(0, 0);
  lcd.print("Water sensor:    ");
  // set the starting point to below the printed word
  lcd.setCursor(2, 1);
  int waterSensorValue = analogRead(A0);
  Serial.println(waterSensorValue);
  callMillis(1000);
  if (waterSensorValue > 50) {
    lcd.print("Detected        ");  //Spaces eliminate need for lcd.clear(), which causes screen flicker, also prevents any text from carrying over between button presses.
  } else {
    lcd.print("Not Detected    ");  //Spaces eliminate need for lcd.clear(), which causes screen flicker, also prevents any text from carrying over between button presses.
  }
  callMillis(500);
}

//On the first button press, activate the photoresistor to detect light intensity of surrounding area, then display on the second line of the LCD
void photoresistorModule() {
  lcd.setCursor(0, 0);
  lcd.print("Light Intensity:");
  lcd.setCursor(2, 1);
  int lightSensorValue = analogRead(A1);
  lcd.print(lightSensorValue);
  callMillis(500);
}

//On the second button press, activate the DHT11 to read the temperature in Farenheit and humidity level of surrounding area, the display on the second line of the LCD
void DHT11_Module() {
  float TempF = (dht.readTemperature() * 9.0) / 5.0 + 32.0;
  float humidity = dht.readHumidity();
  lcd.setCursor(0, 0);
  lcd.print("Humidity:");
  lcd.print(humidity);
  lcd.print("% ");  //Spaces eliminate need for lcd.clear(), which causes screen flicker, also prevents any text from carrying over between button presses.
  lcd.setCursor(2, 1);
  lcd.print("Temp:");
  lcd.print(TempF);
  lcd.print("F");
  callMillis(500);
}

void loop() {
  // detects when the button is pressed
  buttonState = digitalRead(sensorToggleButton);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      numberOfButtonPresses++;
      detectionState = (detectionState + 1) % 2;
    }
    callMillis(50);  // button debounce
  }
  lastButtonState = buttonState;

  // Depending on the number of times the button was pressed, call a different function
  switch (numberOfButtonPresses) {
    case 1:
      photoresistorModule();
      break;
    case 2:
      DHT11_Module();
      break;
    case 3:
      waterSensorModule();
      break;
    case 4:  //clears the number of times you pressed the button, so it defaults to the bootup screen
      numberOfButtonPresses = numberOfButtonPresses % 4;
      lcd.clear();
      break;
    default:
      bootupScreen();
      break;
  }
}
