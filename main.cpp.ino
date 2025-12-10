#include "watersensor.h"
#include "serial.h"
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(6, 7, 8, 9, 10, 11);  // RS, E, D4, D5, D6, D7

RTC_DS1307 rtc; // Create RTC Object

// GPIO Pointers
volatile unsigned char *portDDRB = (unsigned char *) 0x24;
volatile unsigned char *portB =    (unsigned char *) 0x25;
volatile unsigned char *portDDRE = (unsigned char *)0x2D;
volatile unsigned char *portE    = (unsigned char *)0x2E;


unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 50;  // ms


#define DHTPIN 2     // The digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // Specify sensor type as DHT11

// Initialize DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

void DisplayTime(){
  DateTime now = rtc.now();        // read the current time
  
}


void setup(){

  // Set as input pins 13 for toggle button and 12 for reset button and set pins 53, 52, 51, 50 as output for LED
  *portDDRB &= ~((1 << 6) | (1 << 7));

  *portDDRE |= (1 << 4) | (1 << 5);

  // Enable pin
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);


  U0Init(9600);
  adc_init();
  dht.begin();

  // Used for DisplayTime() ISR when toggle button is pressed


  Wire.begin();      // initialize I2C
  rtc.begin();       // initialize RTC communication

  lcd.begin(16, 2);  // 16 col, 2 rows

}

bool readPin(int pinBit) {
  return (PINB & (1 << pinBit)) != 0;
}

bool lastButtonState = false;

bool isPressed(int pinBit) {
    bool currentstate = readPin(pinBit);  

    if (currentstate && !lastButtonState) {
        unsigned long now = millis();
        if (now - lastPressTime > debounceDelay) {
            lastPressTime = now;
            return true;  
        }
    }
    lastButtonState = currentstate;

    return false;
}

bool toggleButtonPressed(){
  // Check PB7 for pin 13
  if(isPressed(7)){
    return true;
  } else {
    return false;
  }
}

bool resetButtonPressed(){
  // Check PB6 for pin 12
  if(isPressed(6)){
    return true;
  } else {
    return false;
  }
}


void displayErrorScreen(){
  lcd.print("Water level is too low");
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

float water_threshold = 200;
float temp_threshold  = 70;


void loop() {

  // Read sensors each loop
  int waterLevel = checklevel(0);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature(true);
  
  resetLEDs();
  turnYellowLED();

  // Universal button: toggle IDLE
  if (toggleButtonPressed()) {
    currentState = IDLE;
  }


  switch (currentState) {
    // ---------------- IDLE STATE ----------------
    case DISABLED:
      // Don't display temperature or humidity
      lcd.clear();
      stopMotor();
      break;

    case IDLE:
      resetLEDs();
      turnGreenLED();
      startMotor();
      showReading(temp, humidity);

      // Enter RUNNING if too hot
      if (temp > temp_threshold) {
        currentState = RUNNING;
      }

      // Enter ERROR if water low
      if (waterLevel <= water_threshold) {
        currentState = ERROR;
      }

      // Stop 
      if (toggleButtonPressed()) {
        currentState = DISABLED;
      }
      
      break;


    // ---------------- RUNNING STATE ----------------
    case RUNNING:
      resetLEDs();
      turnBlueLED();
      startMotor();

      showReading(temp, humidity);

      // Too cool -> stop
      if (temp < temp_threshold) {
        currentState = IDLE;
      }

      // Water low -> error
      if (waterLevel <= water_threshold) {
        currentState = ERROR;
      }

      // Stop 
      if (toggleButtonPressed()) {
        currentState = DISABLED;
        stopMotor();
      }
      break;


    // ---------------- ERROR STATE ----------------
    case ERROR:
      resetLEDs();
      turnRedLED();
      stopMotor();
      displayErrorScreen();

      // Reset button clears error
      if (resetButtonPressed()) {
        currentState = IDLE;
      }

      if(toggleButtonPressed()){
        currentState = DISABLED;
      }

      break;
  }
}

