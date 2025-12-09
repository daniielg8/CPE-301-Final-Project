#include "watersensor.h"
#include "serial.h"
#include "DHT.h"


// GPIO Pointers
volatile unsigned char *portDDRB = (unsigned char *) 0x24;
volatile unsigned char *portB =    (unsigned char *) 0x25;
unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 50;  // ms

bool isIDLE = false; 
bool isRunning = false;
bool waterError = false;
bool Error = false;


#define DHTPIN 2     // The digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // Specify sensor type as DHT11

// Initialize DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

void DisplayTime(){

}

bool readPin(uint8_t 12) {
    return (PIND & (1 << 12)) != 0;
}

bool isPressed(uint8_t 12) {
    bool pressed = !readPin(12);   // active LOW

    if (pressed) {
        unsigned long now = millis();
        if (now - lastPressTime > debounceDelay) {
            lastPressTime = now;
            return true;  
        }
    }
    return false;
}

void setup(){

  // Set pins 13 and 12 as input
  *portDDRB &= ~((1 << 6) | (1 << 7));

  // Set pins 53, 52, 51, 50 as ouput for LED
  *portDDRB |= 0x0F;

  U0Init(9600);
  adc_init();
  dht.begin();
}

void loop(){
  float water_threshold = 200; 
  float temp_threshold = 70;
  
  // Turn on yellow LED

  // Check if buttons is pressed 
  if(){
    isIDLE = true;
  }

  if(isIDLE){
      int waterLevel = checklevel(0);

      float humidity = dht.readHumidity();
      float temp = dht.readTemperature(true);

      // Display temperature and humidity while IDLE to serial
      
      // Check if button is toggled
      if(){
        isIDLE = false;
        return;
      }

      if(temp > temp_threshold){        
        
        isRunning = true;

        while(isRunning){
          // Display temp and humidity on LCD
          humidity = dht.readHumidity();
          temp = dht.readTemperature(true);

          // Turn Blue LED on

          // Start motor

          // Check if temperature has cooled down
          if(temp < temp_threshold){
            isRunning = false;
          }

          // Check if the water level is low
          if(waterLevel < water_threshold){
            waterError = true;
            isRunning = false;
          }

          // When button is toggled to turn off
          if(){
            isIDLE = false;
          }


        }

        // Turn off motor

      }

      if(waterLevel <= water_threshold || waterError){
        Error = true;

        while(Error){
          // Display to LCD and turn on Red LED

          // Check is reset button is pressed
          if(){

          }

          // Check if button is toggled
          if(){
            isIDLE = false;
          }
        }
      }
    
  }
}
