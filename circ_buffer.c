#include <stdint.h>
#include <stdbool.h>
#include "circ_buffer.h"

void cb_init(CB_T *cb)
{
	cb->head = 0;
	cb->tail = 0;
}

bool cb_empty(const CB_T *cb)
{
	return (cb->head == cb->tail);
}

bool cb_full(const CB_T *cb)
{
	return (((cb->head + 1) % CB_BUFFER_SIZE) == cb->tail);
}

void cb_queue(CB_T *cb, char c)
{
	if (!cb_full(cb))
	{
		cb->buffer[cb->head] = c;
		cb->head = (cb->head + 1) % CB_BUFFER_SIZE;
	}
}

char cb_dequeue(CB_T *cb)
{
	if (cb_empty(cb))
	{
		return '\0';	// Return null character since buffer is empty
	}
	else
	{
		char c = cb->buffer[cb->tail];
		cb->tail = (cb->tail + 1) % CB_BUFFER_SIZE;
		return c;
	}
}
