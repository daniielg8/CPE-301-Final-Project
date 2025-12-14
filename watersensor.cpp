// Daniel Garcia and Joaquin Salazar

#include "watersensor.h"
#define RDA 0x80
#define TBE 0x20  
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

void adc_init() {
  // setup the A register
 // set bit 7 to 1 to enable the ADC 
  *my_ADCSRA |= (1 << 7);

 // clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= ~(1 << 5);

 // clear bit 3 to 0 to disable the ADC interrupt 
  *my_ADCSRA &= ~(1 << 3);

 // clear bit 0-2 to 0 to set prescaler selection to slow reading
  *my_ADCSRA &= ~(7 << 0);

  // setup the B register
// clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= ~(1 << 3);

 // clear bit 2-0 to 0 to set free running mode
  *my_ADCSRB &= ~(0b111 << 0);

  // setup the MUX Register
 // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX = 0x00;

// set bit 6 to 1 for AVCC analog reference
  *my_ADMUX |= (1 << 6);

  // clear bit 5 to 0 for right adjust result
  *my_ADMUX &= ~(1 << 5);

 // clear bit 4-0 to 0 to reset the channel and gain bits
  *my_ADMUX &= ~(0b11111 << 0);

}

unsigned int adc_read(unsigned char adc_channel_num) {
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX &= ~(0b11111 << 0); 

  // clear the channel selection bits (MUX 5) hint: it's not in the ADMUX register
  *my_ADCSRB &= ~(1 << 3); 
  
  // set the channel selection bits for channel 0
  *my_ADMUX |= (adc_channel_num & 0x07);

  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= (1 << 6);

  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register and format the data based on right justification (check the lecture slide)
  
  unsigned int val = (*my_ADC_DATA & 0x03FF);
  return val;
}

int checklevel(int channel){
  return adc_read(channel);
}


