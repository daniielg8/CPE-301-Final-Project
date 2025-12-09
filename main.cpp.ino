#include "watersensor.h"
#include "serial.h"
#include "DHT.h"


// GPIO Pointers
volatile unsigned char *portDDRB = (unsigned char *) 0x24;
volatile unsigned char *portB =    (unsigned char *) 0x25;
bool isIDLE = false; 
bool isRunning = false;
bool waterError = false;
bool Error = false

void DisplayTime(){

}

void setup(){
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

      if(Temp > temp_threshold){        
        
        isRunning = true;

        while(isRunning){
          // Display temp and humidity on LCD
          humidity = dht.readHumidity();
          temp = dht.readTemperature(true);

          // Turn Blue LED on

          // Start motor

          // Check if temperature has cooled down
          if(Temp < temp_threshold){
            isRunning = false;
          }

          // Check if the water level is low
          if(waterlevel < water_threshold){
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

      if(waterlevel <= water_threshold || waterError){
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
