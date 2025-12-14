// Daniel Garcia and Joaquin Salazar

#ifndef SERIAL_H
#define SERIAL_H

void U0Init(int U0baud);

unsigned char kbhit();

unsigned char getChar();

void putChar(unsigned char U0pdata);

void putString(const char *s);

void putInt(int n);

void put2(int n);


#endif