#ifndef _UART_H
#define _UART_H

#include <avr/io.h>
#include <stdio.h>
#include <ctype.h> 

#define UART_BAUD_RATE 115200

//#define UART_DOUBLESPEED

#ifdef UART_DOUBLESPEED
#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 4UL)) / ((uint32_t)(baudRate) * 8UL) - 1)
#else
#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 8UL)) / ((uint32_t)(baudRate) * 16UL) - 1)
#endif

void uart_putc(char);
int  uart_printchar(char, FILE *);
void uart_puts (const char *);
void uart_init(void);

int  uart_avail(void);
char uart_getc(void);

#endif
