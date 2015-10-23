#ifndef LINE_BUFFER_H
#define LINE_BUFFER_H

#include <stdbool.h>

// Maximum number of characters to accept on one line
#define LB_BUFFER_SIZE 80

typedef enum {LB_SUCCESS = 0, LB_BUFFER_FULL} LB_STATUS_T;

typedef struct
{
	unsigned int count;
	char buffer[LB_BUFFER_SIZE];
} LB_T;

void lb_init(LB_T *);
bool lb_empty(const LB_T *);
bool lb_full(const LB_T *);
LB_STATUS_T lb_append(LB_T *lb, char);
bool lb_line_ready(const LB_T *);
char * lb_gets_at(LB_T *, unsigned int);
char * lb_gets(LB_T *);

#endif
