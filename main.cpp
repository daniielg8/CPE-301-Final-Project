#include "watersensor.h"
#include "serial.h"


// GPIO Pointers
volatile unsigned char *portDDRB = (unsigned char *) 0x24;
volatile unsigned char *portB =    (unsigned char *) 0x25;
bool isIDLE = false; 


void setup(){
  U0Init(9600);
  adc_init();
}

void loop(){
    if(isIDLE){
      
    }
}

