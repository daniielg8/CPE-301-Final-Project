// Daniel Garcia and Joaquin Salazar

#include "serial.h"
// UART Pointers
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

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

void putString(const char *s) {
  while (*s) {
    putChar(*s++);
  }
}

void put2(int n) {
  if (n < 10) putChar('0');
  putInt(n);
}

void putInt(int n) {
  char buffer[10];
  int i = 0;

  if (n == 0) {
    putChar('0');
    return;
  }

  if (n < 0) {
    putChar('-');
    n = -n;
  }

  while (n > 0) {
    buffer[i++] = '0' + (n % 10);
    n /= 10;
  }

  while (i--) {
    putChar(buffer[i]);
  }
}
