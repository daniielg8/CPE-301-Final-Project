// Daniel Garica
#ifndef WATERSENSOR_H
#define WATERSENSOR_H


void adc_init();

unsigned int adc_read(unsigned char adc_channel_num);

int checklevel(int channel);

#endif

