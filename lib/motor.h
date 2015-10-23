#ifndef _MOTOR_H
#define _MOTOR_H

#include <stdio.h>
#include <stdlib.h>

void motor_init(void);			//Configure ADC and Timer 1
void set_dir1(int dir);
void set_dir2(int dir);
void set_speed1(int speed);
void set_speed2(int speed);
int get_pwm(float V);

typedef struct
{
	float K; // Motor constant [N.m/A] or [V.s/rad]
	float r; // Wheel radius [m]
	float eta; // Gearbox efficiency [-]
	float N; // Gear ratio [-]
	float taum; // Motor static friction torque [N.m]
	float R; // Motor armature resistance [Ohms]
} MOTOR_PARAM_T;

#endif
