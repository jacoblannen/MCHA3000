#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "motor.h"

float control_allocation(float tau);
void update_states(float state_1,float state_2,float state_3);
float controller(void);
void set_ref(float input);
void update_ctrl(float k_1,float k_2,float k_3,float k_4);
void update_state1(float state_1);
void update_state2(float state_2);
void update_state3(float state_3);
float integrator(void);
float get_speed(void);

int pil(void);
float k_x(void);

#endif
