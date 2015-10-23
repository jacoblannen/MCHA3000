#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "motor.h"

// Configure Timer1
void motor_init(void)
{
	DDRA = (1<<DDA1)|(1<<DDA2);
	DDRC = (1<<DDC3)|(1<<DDC4)|(1<<DDC5)|(1<<DDC6);
	DDRD = (1<<DDD4)|(1<<DDD5);													// Configure pins OCA1 and OCB1 as outputs

	PORTA |= (1<<PORTA1)|(1<<PORTA2);
	
	// Set output compares for two different PWM signals
	OCR1AH = 0x00;
	OCR1AL = 0x00;
	OCR1BH = 0x00;
	OCR1BL = 0x00;
	
	ADMUX = (1<<REFS0)|(1<<ADLAR)|(1<<MUX0)|(1<<MUX3);						// Use AVcc for Vref,left align result, and set differential mode with gain of 10
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM10);							// Set Timer 1 to phase correct PWM mode with 8-bit resolution
	TCCR1B = (1<<CS10)|(1<<CS11);														// Use prescaler 1/256
}

void set_dir1(int dir)
{
	if(dir == 1)
	{
		PORTC |= (1<<PORTC3);
		PORTC &= ~(1<<PORTC4);
	}else if(dir == 2)
	{
		PORTC |= (1<<PORTC4);
		PORTC &= ~(1<<PORTC3);
	}else if(dir == 0)
	{
		PORTC &= ~(1<<PORTC3);
		PORTC &= ~(1<<PORTC4);
	}
}

void set_dir2(int dir)
{
	if(dir == 1)
	{
		PORTC |= (1<<PORTC6);
		PORTC &= ~(1<<PORTC5);
	}else if(dir == 2)
	{
		PORTC |= (1<<PORTC5);
		PORTC &= ~(1<<PORTC6);
	}else if(dir == 0)
	{
		PORTC &= ~(1<<PORTC5);
		PORTC &= ~(1<<PORTC6);
	}
}

void set_speed1(int speed)
{
	if(speed<0){
		set_dir1(1);
		speed = speed*-1;
	}else{
		set_dir1(2);
	}
	OCR1AL = speed;
}

void set_speed2(int speed)
{
	if(speed<0){
		set_dir2(1);
		speed = speed*-1;
	}else{
		set_dir2(2);
	}
	
	OCR1BL = speed;
}


int get_pwm(float V)
{
	int pwm;
	
	if(V<-11.5){
		pwm = -255;
	}
	if(V>11.5){
		pwm = 255;
	}else{
		pwm = (int)(V*22.15);
	}

	return(pwm);
}
