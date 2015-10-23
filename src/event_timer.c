#include <avr/io.h>
#include <avr/interrupt.h>
#include "event_timer.h"

static volatile unsigned int event_count; // event counter
static void (*event_func)(void); // pointer to event handler

void event_init(void)
{
	
	TCCR0 = (1<<WGM01)|(1<<COM01)|(1<<CS00)|(1<<CS02);		// Timer 0 in CTC mode, prescaler set to 1/1024
	OCR0 = 0x8F;											// Set OCR0 to trigger at 100Hz
}

void event_reset(void)
{
	event_count = 0;
}

int event_pending(void)
{
	return(event_count);
}

void event_set(void (*func)(void))
{
	event_func = func;
}

void event_action(void)
{
	event_func();
	event_reset();
}

void event_start(void)
{
	TIMSK |= (1<<OCIE0);									// Enable timer 0 compare interrupt
}

void event_stop(void)
{
	TIMSK &= ~(1<<OCIE0);
}

ISR(TIMER0_COMP_vect)
{
	event_count++;
}
