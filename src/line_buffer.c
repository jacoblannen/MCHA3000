#include <stdbool.h>
#include <stddef.h>
#include "line_buffer.h"

void lb_init(LB_T *lb)
{
	lb->count = 0;
	lb->buffer[lb->count] = '\0';
}

bool lb_empty(const LB_T *lb)
{
	return lb->count == 0;
}

bool lb_full(const LB_T *lb)
{
	return lb->count >= LB_BUFFER_SIZE;
}

// Add character to line buffer and report status
LB_STATUS_T lb_putc(LB_T *lb, char c)
{
	if (!lb_full(lb))
	{
		lb->buffer[lb->count++] = c;
		return LB_SUCCESS;
	}
	else
	{
		return LB_BUFFER_FULL;
	}
}

// Remove last character from line buffer
void lb_delc(LB_T *lb)
{
	if (!lb_empty(lb))
	{
		lb->count--;
	}
}

// Append character to line buffer and handle special characters
LB_STATUS_T lb_append(LB_T *lb, char c)
{
	if (c == '\b')
	{
		lb_delc(lb);
	}
	else if (c != '\r') // Ignore carriage return
	{
		if (c != '\n')
		{
			return lb_putc(lb, c);
		}
		else if (!lb_empty(lb))			// Ignore empty line
		{
			return lb_putc(lb, '\0');	// Null-terminate string
		}
	}
	return LB_SUCCESS;
}

// Check if line buffer contains a non-trivial null-terminated string
bool lb_line_ready(const LB_T *lb)
{
	return !lb_empty(lb) && lb->buffer[lb->count-1] == '\0';
}

// Get string starting at specified location
char * lb_gets_at(LB_T *lb, unsigned int i)
{
	if (lb_line_ready(lb) && i < lb->count)
	{
		return &lb->buffer[i];
	}
	else
	{
		return NULL;
	}
}

char * lb_gets(LB_T *lb)
{
	return lb_gets_at(lb, 0);
}
