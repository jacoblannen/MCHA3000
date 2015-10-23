#include <avr/io.h>
#include "adc.h"

void adc_init(void)
{
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);				// Enable ADC with scaling set to 1/64
	ADMUX = (1<<REFS0)|(1<<ADLAR);							// Use internal Vcc for Vref, left align ADC data to only 8 MSB
}

int adc_read(void)
{
	uint16_t out;
	
	while(ADCSRA&(1<<ADSC));
	out = (ADCL>>6);
	out+= (ADCH<<2);
	return(out);
}

float adc_volt(void)
{
	float volt;
	int adc = adc_read();

	volt = (float)adc*5.06/1024;
	
	return(volt);
}
