#include <Keypad.h>
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

//   ->    CONTROL SYSTEM PROJECT
// FASIH ULLAH SALEEM 
// MUHAMMAD ASAD NADEEM
// SHEHERYAR ALI

#include <LiquidCrystal.h>
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
const int flowSensorPin = 2;         // Input pin for the flow sensor
const int pumpPin = 3;               // Output pin for controlling the water pump
volatile unsigned int pulseCount;    // Counter for flow sensor pulses
unsigned long flowInterval;          // Interval for calculating flow rate
float flowRate;                      // Flow rate in mL/min
unsigned long totalMilliLitres = 0;  // Total volume flowed in milliliters
unsigned long volume = 0;            // Target volume in milliliters
unsigned long oldTime = 0;           // Variable to store the last time flow rate was calculated
byte check = 0;
byte bottle = 0;

#define buzzerPin 12
#define sensorIR 13

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

/// NEW CONFIGURATION
byte pin_rows[ROW_NUM] = { 11, 10, 9, 8 };
byte pin_column[COLUMN_NUM] = { 7, 6, 5, 4 };

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

String code = "";

void setup() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  pinMode(sensorIR, INPUT_PULLUP);
  pinMode(flowSensorPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);  // Ensure pump is initially off (assuming LOW signal turns on the pump)
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Control Project");
  lcd.setCursor(0, 1);
  lcd.print(" 263, 267, 326");
  delay(3500);
  lcd.clear();
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, FALLING);
  pulseCount = 0;
  flowInterval = 1000;  // Interval for calculating flow rate (1 second)
}

void loop() {
  bottle = digitalRead(sensorIR);
  switch (bottle) {

    case HIGH:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NO BOTTLE");
      delay(100);
      lcd.clear();
      digitalWrite(pumpPin, LOW);
      break;

    case LOW:
      if(!volume){
      lcd.setCursor(0, 0);
      lcd.print("Set Volume:");
      char key = keypad.getKey();

      if (key) {
        code += key;
        lcd.setCursor(0, 1);
        lcd.print(code);
        delay(100);
        if (key == '*') {
        code = "";
        lcd.clear();
      }
      }

      if (key == 'D') {
        if (code.toInt() <= 2000) {
          volume = code.toInt();
        } 
        else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Set Volume:");
        }
        code = "";
      }
      

      }
      if(volume && !check ){
      if ((millis() - oldTime) > 1000) {  // Update flow rate and total volume every second
        // Calculate flow rate
        flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / 5.7;  // Flow rate in mL/min

        // Calculate volume flowed during this interval
        unsigned long intervalVolume = (flowRate * 1000) / 60;  // Volume in milliliters

        // Update total volume
        totalMilliLitres += intervalVolume;
        
        // Turn off the pump if total volume reaches the target volume
        if (totalMilliLitres >= volume) {
          digitalWrite(pumpPin, LOW);  // Turn off the pump (assuming LOW signal turns on the pump)
          Serial.println("Target volume reached. Pump turned off.");
          check = 1;
          digitalWrite(buzzerPin, HIGH);
          delay(2000);
          digitalWrite(buzzerPin, LOW);
        } else {
          // Turn on the pump if flow rate is below a certain threshold (e.g., 200 mL/min)
          if (flowRate < 5) {             // Adjust threshold as needed
            digitalWrite(pumpPin, HIGH);  // Turn on the pump (assuming LOW signal turns on the pump)
          } else {
            digitalWrite(pumpPin, LOW);  // Turn off the pump (assuming LOW signal turns on the pump)
          }
        }

        lcd.setCursor(0, 0);
        lcd.print("Flowrate:");
        lcd.print(flowRate);
        lcd.print("mL/min");
        lcd.setCursor(0, 1);
        lcd.print("Totalvolume:");
        lcd.print(totalMilliLitres);
        lcd.print("mL");

        // Reset pulse count and time for next interval
        pulseCount = 0;
        oldTime = millis();
      }
      }
  }
}
void pulseCounter() {
  pulseCount++;
}
