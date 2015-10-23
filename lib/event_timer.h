#ifndef _EVENT_TIMER_H
#define _EVENT_TIMER_H

void event_init(void);
void event_reset(void);
int event_pending(void);
void event_set(void (*func)(void));
void event_action(void);
void event_start(void);
void event_stop(void);

#endif
