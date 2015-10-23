#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h> 

#include "uart.h"
#include "line_buffer.h"
#include "circ_buffer.h"
#include "encoder.h"
#include "cmd.h"
#include "controller.h"
#include "motor.h"
#include "i2cmaster.h"
#include "imu.h"
#include "event_timer.h"
#include "adc.h"
#include "kalman.h"

static LB_T lb;
float time = 0;

int main(void)
{

	lb_init(&lb);
	event_init();
	motor_init();
	uart_init(); 	// init USART
	enc_init();
	i2c_init();
	adc_init();
	kalman_init();
	sei();  		// enable interrupts


	// Wait a second at startup
	_delay_ms(1000);

	// send initial string
	printf_P(PSTR("Hello world!\n"));
	imu_init();

	for (;/*ever*/;)
	{
//		ADCSRA |= (1<<ADSC);								// Set start conversion bit and wait for conversion to finish
//		while(ADCSRA&(1<<ADSC));

//		OCR1AL = ADCH;										// Set ADC reading to timer 0 compare

		if(event_pending())
		{
			event_action();
		}
		else // No pending operation, do low priority tasks
		{
			// dequeue receive buffer if any bytes waiting
			while (uart_avail())
			{
				char c = uart_getc();
				if (lb_append(&lb, c) == LB_BUFFER_FULL)
				{
					lb_init(&lb); // Clear line
					printf_P(PSTR("\nMax line length exceeded\n"));
				}
				// Process command if line buffer is ready ...
				if (lb_line_ready(&lb))
				{
					strcpy(cmd_string,lb_gets(&lb));
					do_cmd(cmd_string);
					lb_init(&lb);
				}
			}
		}
		// Process command if line buffer is terminated by a line feed or carriage return
	}
	return 0;
}
