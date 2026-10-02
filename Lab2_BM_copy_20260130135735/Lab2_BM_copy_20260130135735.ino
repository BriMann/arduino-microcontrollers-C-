/*
Module 2 lab, LED Sequence Comparitors
Brian Mann
January 30, 2026
This sketch will sequentially turn ON and OFF three LEDs using a mathematical comparison on an incremented variable
*/
const int RLED = 9;  // Sets red LED to integer 9
const int YLED = 8;  // Sets yellow LED to integer 8
const int GLED = 7;  // Sets green LED to integer 7
int INC = 0;         // INC is the increment variable


void setup() {
  pinMode(YLED, OUTPUT);  // sets pin 8 as an output
  pinMode(RLED, OUTPUT);  // sets pin 9 as an output
  pinMode(GLED, OUTPUT);  // sets pin 7 as an output
  Serial.begin(9600);
}

void loop() {

  INC++;  // Increments INC every time the loop is executed

  if (INC < 10) {  // If INC is less than 10, light the red diode
    digitalWrite(RLED, HIGH);
    Serial.println("if");
    delay(100);
  }

  else if (INC < 20) {  // If INC is greater than 9 but less than 20, turn off red and light yellow
    digitalWrite(RLED, LOW);
    digitalWrite(YLED, HIGH);
    Serial.println("else if");
    delay(100);
  }

  else if (INC < 30) { //IfINC is greater than 19 but les than 30, turn off yellow and ligh green
    digitalWrite(YLED, LOW);
    digitalWrite(GLED, HIGH);
    Serial.println("else if");
    delay(100);
  }

  else {
    INC = 0;                  // Reset INC to 0 for next run thru
    digitalWrite(GLED, LOW);  // Turn green off
    Serial.println("else");
    delay(500);
  }
}