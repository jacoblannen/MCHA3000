#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "encoder.h"

static int64_t enc1_count;
static int64_t enc2_count;

void enc_init(void)
{
	MCUCR |= (1 << ISC00) | (1 << ISC10);
	GICR |= (1 << INT1) | (1 << INT0);

	enc1_count = 0;
	enc2_count = 0;
}

float enc1_read(void)
{
	return(enc_angles(enc1_count));
}

float enc2_read(void)
{
	return(enc_angles(enc2_count));
}

void enc_reset(void)
{
	enc1_count = 0;
	enc2_count = 0;
}

float enc_angles(int encoder)
{
	float ret = (float)encoder*((2*PI)/960);
	return(ret);
}

ISR(INT0_vect)		// INT0 interrupt
{				
	if((PIND&(1<<PIND2))>>PIND2==(PINA&(1<<PINA3))>>PINA3)
	{
		enc1_count++;
	}else
	{
		enc1_count--;
	}
}

ISR(INT1_vect)		// INT1 interrupt
{				
	if((PIND&(1<<PIND3))>>PIND3==(PINA&(1<<PINA4))>>PINA4)
	{
		enc2_count--;
	}else
	{
		enc2_count++;
	}
}
