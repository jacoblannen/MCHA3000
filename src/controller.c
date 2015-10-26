#include "controller.h"
#include "motor.h"
#include "get_controller.h"
#include "get_params.h"
#include "kalman.h"
#include "encoder.h"
#include <avr/pgmspace.h> 

#define STEP 0.01

static float state[4];
static b_struct_T parameters;
static float ref = 0;
static float N = 0.1801;
static float K[] = {-0.7249,-24.2260,-4.6940,2.5856};

static MOTOR_PARAM_T motor = {.K = 0.0102, .r = 0.04, .eta = 0.5, .N = 30, .taum = 0.008, .R = 2.55};


void control_init(void)
{
	get_params(&parameters);
	state[0] = 0;
	state[1] = 0;
	state[2] = 0;
	state[3] = 0;
}


float control_allocation(float tau)
{
	float V = 0;

	if(tau>0){
		V = ((tau)/(motor.eta*motor.N*motor.K) + (motor.taum/motor.K))*motor.R+((motor.K*motor.N*state[0]));
	}else if(tau<0){
		V = ((tau)/(motor.eta*motor.N*motor.K) - (motor.taum/motor.K))*motor.R+((motor.K*motor.N*state[0]));
	}else{
		V = motor.K*motor.N*state[0];
	}

	return(V);
}


void set_ref(float input)
{
	ref = input;
}

float controller(void)
{
	float control;
	kalman_state val = kalman_values();

	state[0] = get_speed();
	state[1] = val.x_angle;
	state[2] = val.x_rate;
	state[3] = integrator();

	control = -N*state[3]-(state[0]*K[0]+state[1]*K[1]+state[2]*K[2]);

	return(control);
}

void update_ctrl(float k_1,float k_2,float k_3,float k_4)
{
	N = k_4;
	K[0] = k_1;
	K[1] = k_2;
	K[2] = k_3;
	K[3] = k_4;
}

void update_states(float state_1,float state_2,float state_3)
{
	state[0] = state_1;
	state[1] = state_2;
	state[2] = state_3;
}


void update_state1(float state_1)
{
	state[0] = state_1;
}

void update_state2(float state_2)
{
	state[1] = state_2;
}

void update_state3(float state_3)
{
	state[2] = state_3;
}

float k_x(void)
{
	float err;
	
	err = (ref - (state[0] + state[2]));

	state[3]+= err;

	return(state[0]*K[0]+state[1]*K[1]+state[2]*K[2]);
}

float pil(void)
{
	float out;
	int pwm;
	
	out = control_allocation(controller());
	pwm = get_pwm(out);
	set_speed1(-pwm);
	set_speed2(pwm);

	return((float)pwm);
}

float integrator(void)
{
	static float err = 0;
	static float previous = 0;

	previous+=err*STEP;

	err = ref - (state[0] + state[2]);

	return(previous);
}

float get_speed(void)
{
	static float current = 0;
	static float previous;

	previous = current;
	current = enc1_read();

	return((current-previous)/STEP);
}

float check_angle(void)
{
	return(state[1]);
}
