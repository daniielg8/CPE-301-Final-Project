// Daniel Garcia and Joaquin Salazar

#include "watersensor.h"
#include "serial.h"
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>
#include <Stepper.h>

const int stepsPerRevolution = 2048; // 28BYJ-48 full revolution

// Use your pins: 31, 33, 35, 37
Stepper stepper(stepsPerRevolution, 31, 33, 35, 37);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(6, 7, 8, 9, 10, 11);  // RS, E, D4, D5, D6, D7

RTC_DS1307 rtc; // Create RTC Object

// GPIO Pointers
volatile unsigned char *portDDRG  = (unsigned char *)0x33;
volatile unsigned char *portG = (unsigned char *)0x34;

volatile unsigned char *portDDRB = (unsigned char *) 0x24;
volatile unsigned char *portB =    (unsigned char *) 0x25;
volatile unsigned char *portDDRE = (unsigned char *)0x2D;
volatile unsigned char *portE    = (unsigned char *)0x2E;


unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 50;  // ms


#define DHTPIN 47     // The digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // Specify sensor type as DHT11

// Initialize DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

#define MAX_STEPS 512    // Adjust based on vent travel
#define DEAD_BAND 4      // ignore tiny movements
#define END_ZONE 15      // pot zone near 0 & 1023

void moveVent() {
  static int lastStepPos = -1;

  int potVal = adc_read(1);  // 0–1023

  // ----- Clamp ends -----
  if (potVal < END_ZONE) {
    potVal = 0;
  } 
  else if (potVal > (1023 - END_ZONE)) {
    potVal = 1023;
  }

  int targetStepPos = map(potVal, 0, 1023, 0, MAX_STEPS);

  if (lastStepPos == -1) {
    lastStepPos = targetStepPos;
    return;
  }

  int stepDelta = targetStepPos - lastStepPos;

  // ----- Strong deadband -----
  if (abs(stepDelta) <= DEAD_BAND) {
    return;   // FULL STOP
  }

  stepper.step(stepDelta);
  lastStepPos = targetStepPos;
}

void printDate(DateTime now) {
  putString("Date: ");
  put2(now.day());
  putChar('/');
  put2(now.month());
  putChar('/');
  putInt(now.year());
  putChar('\n');
}


void printTime(DateTime now) {
  putString("Time: ");
  put2(now.hour());
  putChar(':');
  put2(now.minute());
  putChar(':');
  put2(now.second());
  putChar('\n');
}

void printTimestamp(DateTime now) {
  putString("Timestamp: ");

  // YYYY-MM-DD
  putInt(now.year());
  putChar('-');
  put2(now.month());
  putChar('-');
  put2(now.day());
  putChar(' ');

  // HH:MM:SS
  put2(now.hour());
  putChar(':');
  put2(now.minute());
  putChar(':');
  put2(now.second());

  putChar('\n');
}


void setup(){
  // Set as input pins 13 for toggle button and 12 for reset button and set pins 53, 52, 51, 50 as output for LED
  *portDDRB &= ~((1 << 6) | (1 << 7));
  *portB |= (1 << 6) | (1 << 7);         // enable pullups!

  *portDDRE |= (1 << 4) | (1 << 5);

  // Enable pin
 *portDDRG |= (1 << 5);
  *portG |= (1 << 5);

  U0Init(9600);
  adc_init();
  dht.begin();


  Wire.begin();      // initialize I2C
  rtc.begin();       // initialize RTC communication

  if (!rtc.isrunning()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.begin(16, 2);  // 16 col, 2 rows
  stepper.setSpeed(10);

}

bool readPin(int pinBit) {
  return (PINB & (1 << pinBit)) != 0;
}

bool lastToggleState = false;
bool lastResetState = false;

bool isPressed(int pinBit, bool &lastState) {
    bool current = readPin(pinBit);

    if (current && !lastState) {
        unsigned long now = millis();
        if (now - lastPressTime > debounceDelay) {
            lastPressTime = now;
            lastState = current;
            return true;
        }
    }

    lastState = current;
    return false;
}

bool toggleButtonPressed(){
  // Check PB7 for pin 13
  if(isPressed(7, lastToggleState)){
    return true;
  } else {
    return false;
  }
}

bool resetButtonPressed(){
  // Check PB6 for pin 12
  if(isPressed(6, lastResetState)){
    return true;
  } else {
    return false;
  }
}


void displayErrorScreen(){
  lcd.setCursor(0, 0);
  lcd.print("Water level is");
  lcd.setCursor(0, 1);
  lcd.print("too low");
}

void showReading(float temp, float humidity){
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
}


void resetLEDs(){
  *portB &= 0b0000;
}

void turnYellowLED(){
  // turn on pin 50
  *portB |= 0b1000;
}

void turnBlueLED(){
  // turn on 51
  *portB |= 0b0100;
}

void turnRedLED(){
  // turn on 52
  *portB |= 0b0010;
}

void turnGreenLED(){
  // turn on 53
  *portB |= 0b0001;
}


void startMotor(){
  *portE |= (1 << 4);        // IN1 = HIGH
  *portE &= ~(1 << 5);       // IN2 = LOW
}

void stopMotor(){
  *portE &= ~(1 << 4);       // IN1 = LOW
  *portE &= ~(1 << 5);       // IN2 = LOW
}

enum State { DISABLED, IDLE, RUNNING, ERROR };
State currentState = DISABLED;

float water_threshold = 240;
float temp_threshold  = 78;



void loop() {
  DateTime now = rtc.now();

  // Read sensors each loop
  int waterLevel = checklevel(0);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature(true);

  // Universal button: toggle IDLE
  if (toggleButtonPressed()) {
    currentState = IDLE;
  }

  switch (currentState) {
    case DISABLED:
      // Don't display temperature or humidity
      lcd.clear();
      stopMotor();
      resetLEDs();
      turnYellowLED();
      break;

    case IDLE:
      showReading(temp, humidity);     
      stopMotor();
      moveVent();

      // Enter RUNNING if too hot
      if (temp > temp_threshold) {
        printTimestamp(now);
        currentState = RUNNING;
      }

      // Enter ERROR if water low
      if (waterLevel <= water_threshold) {
        lcd.clear();
        printTimestamp(now);
        currentState = ERROR;
      }

      // Stop 
      if (toggleButtonPressed()) {
        printTimestamp(now);
        currentState = DISABLED;
      }
      
      resetLEDs();
      turnGreenLED();
      break;

    case RUNNING:
      moveVent();
      showReading(temp, humidity);

      // Too cool -> stop
      if (temp < temp_threshold) {
        printTimestamp(now);
        stopMotor();

        currentState = IDLE;
      }
      
      // Water low -> error
      if (waterLevel <= water_threshold) {
        printTimestamp(now);
        lcd.clear();
        currentState = ERROR;
      }

      // Stop 
      if (toggleButtonPressed()) {
        printTimestamp(now);
        currentState = DISABLED;
      }

      resetLEDs();
      turnBlueLED();
      startMotor();
      break;

    case ERROR:
      resetLEDs();
      turnRedLED();
      stopMotor();
      moveVent();
      displayErrorScreen();     // show it

      // Reset button clears error
      if (resetButtonPressed()) {
        printTimestamp(now);
        lcd.clear();
        currentState = IDLE;
      }

      if (toggleButtonPressed()) {
        printTimestamp(now);
        lcd.clear();
        currentState = DISABLED;
      }

      break;
  }
}
