#include "uart.h"
#include <avr/interrupt.h>
#include "circ_buffer.h"

// Define and initialise stream used for printing to uart
FILE uart_str = FDEV_SETUP_STREAM(uart_printchar, NULL, _FDEV_SETUP_RW);

static CB_T rxb;
static CB_T txb;

void uart_putc(char c)
{
	while(cb_full(&txb));	// Wait until TX buffer has space
	cb_queue(&txb, c);
	UCSRB |= (1 << UDRIE);
}

int uart_printchar(char c, FILE * stream)
{
	uart_putc(c);
	return 0;
}

void uart_puts (const char *s)
{
	// Loop until end of string
	while (*s) uart_putc(*s++);
}

void uart_init(void)
{
	// Set baud rate
	UBRRH = (uint8_t)(UART_CALC_BAUDRATE(UART_BAUD_RATE)>>8);
	UBRRL = (uint8_t)UART_CALC_BAUDRATE(UART_BAUD_RATE);

#ifdef UART_DOUBLESPEED
	UCSRA = (1 << U2X);
#endif

	// Enable receiver and transmitter; enable RX interrupt
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);

	// Asynchronous 8N1
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
	
	// Redirect stdout, so that printf() knows where to go
	stdout = &uart_str;

	// Initialise RX buffer
	cb_init(&rxb);
	cb_init(&txb);
}

int uart_avail(void)
{
	// Return 0 if RX buffer is empty
	return(!cb_empty(&rxb));
}

char uart_getc(void)
{
	return(cb_dequeue(&rxb));
}

ISR(USART_RXC_vect)		// USART RX interrupt
{				
	cb_queue(&rxb, UDR);
}

ISR(USART_UDRE_vect)		// USART RX interrupt
{				
	if(!cb_empty(&txb)){
		UDR = cb_dequeue(&txb);
	}else{
		UCSRB &= ~(1 << UDRIE);
	}
}
