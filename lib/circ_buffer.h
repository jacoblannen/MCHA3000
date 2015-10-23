#ifndef _CIRC_BUFFER_H
#define _CIRC_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define CB_BUFFER_SIZE 32

typedef struct
{
	uint16_t head, tail;			// Head and tail indices
	char buffer[CB_BUFFER_SIZE];
} CB_T;

void cb_init(CB_T *);
bool cb_empty(const CB_T *);
bool cb_full (const CB_T *);
void cb_queue(CB_T *, char);
char cb_dequeue(CB_T *);

#endif
