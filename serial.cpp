
#include "serial.h"

void U0Init(int U0baud){
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20; // Set transmit buffer to be ready to receive new data 
 *myUCSR0B = 0x18; // Enable transmit and receive
 *myUCSR0C = 0x06; // Set number of data bits
 *myUBRR0  = tbaud;
 
}

unsigned char kbhit(){
  return(*myUCSR0A & 0x80); // If transmit buffer has received
}

unsigned char getChar(){
  return *myUDR0;
}

void putChar(unsigned char U0pdata){
  while(!(*myUCSR0A & 0x20)); // Check if transmit buffer is empty
  *myUDR0 = U0pdata;
}