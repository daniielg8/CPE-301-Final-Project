#ifndef SERIAL_H
#define SERIAL_H

void U0Init(int U0baud);

unsigned char kbhit();

unsigned char getChar();

void putChar(unsigned char U0pdata);

#endif