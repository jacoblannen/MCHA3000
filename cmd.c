#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h> 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "cmd.h"
#include "encoder.h"
#include "uart.h"
#include "line_buffer.h"
#include "motor.h"
#include "controller.h"
#include "imu.h"
#include "event_timer.h"
#include "adc.h"
#include "kalman.h"

const CMD_T cmd_table[] = 
{
	{"help",		cmd_help,			"Displays this command list"},
	{"ctrl?",		cmd_controller,		"Displays the control signal"},
	{"enc1_read",	cmd_enc1_read,		"Displays the value of enc1_count"},
	{"enc2_read",	cmd_enc2_read,		"Displays the value of enc2_count"},
	{"enc_reset",	cmd_enc_reset,		"Resets the value of enc_count"},
	{"pinout",		cmd_pinout,			"Displays pin-out diagram for Atmega32A"},
	{"speed",		cmd_set_speed, 		"Sets the speed of both motors"},
	{"dir_1",		cmd_set_dir1,		"Sets direction of motor 2 to \"forward\" or \"back\""},
	{"speed_1",		cmd_set_speed1, 	"Sets the speed of motor 2"},
	{"dir_2",		cmd_set_dir2,		"Sets direction of motor 2 to \"forward\" or \"back\""},
	{"speed_2",		cmd_set_speed2, 	"Sets the speed of motor 2"},
	{"imu",			cmd_imu,			"Grabs a reading from the IMU"},
	{"imu_log",		cmd_imu_log,		"Log nsamples of IMU data"},
	{"enc_log",		cmd_enc_log,		"Log nsamples of enc data"},
	{"adc_log",		cmd_adc0_log,		"Log nsamples of adc0 data"},
	{"motor_log",	cmd_motor_log,		"Log nsamples of data from ADC and encoders"},
	{"tune",		cmd_kalman_tune,	"Tune kalman filter parameters dynamically"},
	{"kal_log",		cmd_kalman_log,		"Log nsamples of data from kalman filter"},
	{"kal",			cmd_kal,			"Retrieve one set of data from kalman filter"},
	{"ref",			cmd_set_ref,		"Set reference for controller"},
	{"states",		cmd_update_states,	"Update states for PIL/HIL"},
	{"state1",		cmd_update_state1,	"Update state 1 for PIL/HIL"},
	{"state2",		cmd_update_state2,	"Update state 2 for PIL/HIL"},
	{"state3",		cmd_update_state3,	"Update state 3 for PIL/HIL"},
	{"update_k",	cmd_update_ctrl,	"Update K gains"},
	{"pil?",		cmd_pil,			"Return values for PIL"},
	{"balance",		cmd_balance,		"Balance dammit"},
	{NULL,			NULL,				NULL}
};

int log_count;
static int nsamples;
static int count;

char string_parser(char* inp, char* array_of_words[]){
	char buffer[50];													//Buffer string used to retrieve single word at a time to be placed into array
	int i=0;															//Loop counters
	int j=0;
	int k=0;

	if(inp<0||array_of_words<0){										//Check pointers contain valid memory addresses
		printf("Please ensure pointers point to valid memory address.\n");
		return(0);
	}

	while(inp[i]!=0){													//While loop till end of input string (if input is NULL the loop ends)
		if(inp[i]!=' '&&inp[i]!='='){												//If current char in input string is not a space move it into the buffer and increment indicies
			buffer[k] = inp[i];
			i++;
			k++;
		}else{
			buffer[k]='\0';												//If input char is a space null terminate buffer string
			array_of_words[j] = malloc(k+1);								//Initialise string to required length and move buffer in.
			strcpy(array_of_words[j],buffer);

			if(i>0){													//Increment word count ignoring leading space
				j++;
			}
			while(inp[i]==' '||inp[i]=='='){											//Check for multiple spaces
				i++;
			}

			k=0;														//Reset buffer index for new word
		}
	}

	if(i!=0){															//After end of input string is detected check that it wasn't empty
		if(inp[i-1]!=' '&&inp[i]!='='){												//Ensure last char was not a space
			buffer[k]='\0';												//Move final buffer into array and increment word count
			array_of_words[j] = malloc(k+1);
			strcpy(array_of_words[j],buffer);
			j++;
		}
	}else{																//Display error if string is empty
		printf("ERROR: Input string contains starts with NULL.\n");
	}

	return(j);															//Return wordcount
}


void do_cmd(char* cmd_string)
{
	int i;
	arg_count = string_parser(cmd_string,out_array); //Use string_parser() to separate arguments of cmd_string
	for(i=0;i<strlen(out_array[0]);i++)
	{
		out_array[0][i]=tolower(out_array[0][i]);
	}
	cmd_flag=0;
	for(i=0; cmd_table[i].cmd!=NULL; i++)
	{
		if(strcmp(cmd_table[i].cmd, out_array[0])==0)
		{
			cmd_table[i].func(arg_count,out_array); //Run function with command line arguments
			cmd_flag=1;
		}
	}
	if(cmd_flag==0)
	{
		printf_P(PSTR("\nERROR: Invalid command. Use 'help' command to view list of valid commands.\n"));
	}
	for(i=0;i<arg_count;i++)
	{
		free(out_array[i]);
	}

}

void cmd_help(int argc, char *argv[])
{
	printf_P(PSTR(
		"Command        Description\n"
		"--------------------------\n"
	));
	for (int i = 0; cmd_table[i].cmd!=NULL; i++)
	{
		printf_P(PSTR("%-14s %s\n"), cmd_table[i].cmd, cmd_table[i].help);
	}
}

void cmd_enc1_read(int argc, char *argv[])
{
	printf_P(PSTR("enc1 set to %d\n"), enc1_read());
}

void cmd_enc2_read(int argc, char *argv[])
{
	printf_P(PSTR("enc2 set to %f\n"), enc2_read());
}

void cmd_enc_reset(int argc, char *argv[])
{
	enc_reset();
	printf_P(PSTR("enc reset to 0\n"));
}

void cmd_getctrl(int argc, char *argv[])
{
//	float angleref = velocity_controller(velref-vel);
//	float ctrl = angle_controller(angleref-theta);
//	printf_P(PSTR("%g\n"), ctrl);
}

void cmd_pinout(int argc, char *argv[])
{
	printf_P(PSTR("ATmega32 pin configuration:\n"));
	printf_P(PSTR("                (XCK/T0)PB0     <-> 1   +----\\_/----+   40 <-- PA0(ADC0)  :Angle potentiometer\n"));
	printf_P(PSTR("                (T1)PB1         <-> 2   |           |   39 --> PA1(ADC1)  :Motor 2 EN/DIAG\n"));
	printf_P(PSTR("                (INT2/AIN0)PB2  <-> 3   |     A     |   38 --> PA2(ADC2)  :Motor 1 EN/DIAG\n"));
	printf_P(PSTR("                (OC0/AIN1)PB3   <-> 4   |     T     |   37 <-- PA3(ADC3)  :Motor 1 enc B\n"));
	printf_P(PSTR("                (!SS)PB4        <-> 5   |     M     |   36 <-- PA4(ADC4)  :Motor 2 enc B\n"));
	printf_P(PSTR("                (MOSI)PB5       <-> 6   |     E     |   35 <-> PA5(ADC5)\n"));
	printf_P(PSTR("                (MISO)PB6       <-> 7   |     L     |   34 <-> PA6(ADC6)\n"));
	printf_P(PSTR("                (SCK)PB7        <-> 8   |           |   33 <-> PA7(ADC7)\n"));
	printf_P(PSTR("                !RESET          --> 9   |     A     |   32 --- AREF\n"));
	printf_P(PSTR("                VCC             --- 10  |     T     |   31 --- GND\n"));
	printf_P(PSTR("                GND             --- 11  |     m     |   30 <-- AVCC       :VCC\n"));
	printf_P(PSTR("                XTAL2           <-- 12  |     e     |   29 <-> PC7(TOSC2)\n"));
	printf_P(PSTR("                XTAL1           --> 13  |     g     |   28 --> PC6(TOSC1) :Motor 2 INA\n"));
	printf_P(PSTR("                (RXD)PD0        --> 14  |     a     |   27 --> PC5(TDI)   :Motor 2 INB\n"));
	printf_P(PSTR("                (TXD)PD1        <-- 15  |     3     |   26 --> PC4(TDO)   :Motor 1 INB\n"));
	printf_P(PSTR(" Motor 1 enc A: (INT0)PD2       --> 16  |     2     |   25 --> PC3(TMS)   :Motor 1 INA\n"));
	printf_P(PSTR(" Motor 2 enc A: (INT1)PD3       --> 17  |           |   24 <-- PC2(TCK)   :Bootloader\n"));
	printf_P(PSTR(" Motor 2 PWM:   (OC1B)PD4       <-- 18  |           |   23 <-- PC1(SDA)   :IMU\n"));
	printf_P(PSTR(" Motor 1 PWM:   (OC1A)PD5       <-- 19  |           |   22 <-- PC0(SCL)   :IMU\n"));
	printf_P(PSTR("                (ICP1)PD6       <-> 20  +-----------+   21 <-> PD7(OC2)\n"));
}

void cmd_set_speed1(int argc, char* argv[])
{
	int speed = atof(argv[1]);
	if(speed<0){
		set_dir1(1);
		speed = speed*-1;
	}else{
		set_dir1(2);
	}
	OCR1AL = speed;
}

void cmd_set_speed2(int argc, char* argv[])
{
	int speed = atof(argv[1]);
	if(speed<0){
		set_dir2(1);
		speed = speed*-1;
	}else{
		set_dir2(2);
	}
	OCR1BL = speed;
}

void cmd_set_speed(int argc, char* argv[])
{
	int speed = atof(argv[1]);

	set_speed1(-speed);
	set_speed2(speed);
}

void cmd_set_dir1(int argc, char* argv[])
{
	int dir = atoi(argv[1]);

	if(dir == 1)
	{
		PORTC |= (1<<PORTC3);
		PORTC &= ~(1<<PORTC4);
	}else if(dir == 2)
	{
		PORTC |= (1<<PORTC3);
		PORTC &= ~(1<<PORTC4);
	}else if(dir == 0)
	{
		PORTC &= ~(1<<PORTC3);
		PORTC &= ~(1<<PORTC4);
	}
}

void cmd_set_dir2(int argc, char* argv[])
{
	int dir = atoi(argv[1]);
	printf_P(PSTR("test %s %i\n"),argv[1],dir);

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

void cmd_imu(int argc, char* argv[])
{
	imu_val results;

	results = imu_read();
	printf_P(PSTR("X-ACC: %d\nY-ACC: %d\nZ-ACC: %d\nX-GYR: %d\nY-GYR: %d\nZ-GYR: %d\n"),results.accx,results.accy,results.accz,results.gyrox,results.gyroy,results.gyroz);
}

void cmd_imu_log(int argc, char* argv[])
{
	nsamples = atoi(argv[1]);
	count = 0;

	printf_P(PSTR("Time,X-ACC,Y-ACC,Z-ACC,X-GYR,Y-GYR,Z-GYR\n"));
	event_set(event_imu_log);
	event_start();
}

void event_imu_log(void)
{
	float time = (float)count/100;

	if(nsamples){
		imu_val results=imu_read();
		printf_P(PSTR("%.2f,%d,%d,%d,%d,%d,%d\n"),time,results.accx,results.accy,results.accz,results.gyrox,results.gyroy,results.gyroz);
		count++;
		nsamples--;
	}else{
		event_stop();
	}
}

void cmd_enc_log(int argc, char* argv[])
{
	nsamples = atoi(argv[1]);
	count = 0;
	enc_reset();

	printf_P(PSTR("Time,Motor1(rad),Motor2(rad)\n"));
	event_set(event_enc_log);
	event_start();
}

void event_enc_log(void)
{
	float time = (float)count/100;
	if(nsamples){
		printf_P(PSTR("%.2f,%f,%f\n"),time,enc1_read(),enc2_read());
		count++;
		nsamples--;
	}else{
		event_stop();
	}
}

void cmd_adc0_log(int argc, char* argv[])
{
	nsamples = atoi(argv[1]);
	count = 0;
	enc_reset();

	printf_P(PSTR("Time,ADC\n"));
	event_set(event_adc0_log);
	event_start();
}

void event_adc0_log(void)
{
	float time = (float)count/100;

	if(nsamples){
		ADCSRA |= (1<<ADSC);
		printf_P(PSTR("%.2f,%f\n"),time,adc_volt());
		count++;
		nsamples--;
	}else{
		event_stop();
	}

}

void cmd_motor_log(int argc, char* argv[])
{
	nsamples = atoi(argv[1]);
	count = 0;
	enc_reset();

	printf_P(PSTR("Time,ADC,Motor1(rad),Motor2(rad)\n"));
	event_set(event_motor_log);
	event_start();
}

void event_motor_log(void)
{
	float time = (float)count/100;

	if(nsamples){
		ADCSRA |= (1<<ADSC);
		printf_P(PSTR("%.2f,%f,%f,%f\n"),time,adc_volt(),enc1_read(),enc2_read());
		count++;
		nsamples--;
	}else{
		event_stop();
	}

}

void cmd_kalman_tune(int argc, char* argv[])
{
	double R_angle = atof(argv[1]);
	double R_gyro = atof(argv[2]);
	double Q_angle = atof(argv[3]);
	double Q_rate = atof(argv[4]);
	double Q_bias = atof(argv[5]);

	kalman_tune(R_angle,R_gyro,Q_angle,Q_rate,Q_bias);
}

void cmd_kalman_log(int argc, char* argv[])
{
	nsamples = atoi(argv[1]);
	enc_reset();
	kalman_reset();
	count = 0;

	printf_P(PSTR("Time,Angle(rad),Gyro(rad/s),Enc_angle\n"));
	event_set(event_kalman_log);
	event_start();
}

void event_kalman_log(void)
{
	float time = (float)count/100;

	if(nsamples){
		kalman_state val = kalman_values();
		printf_P(PSTR("%.2f,%lf,%lf,%f\n"),time,val.x_angle,val.x_rate,enc1_read());
		count++;
		nsamples--;
	}else{
		event_stop();
	}
}

void cmd_controller(int argc, char* argv[])
{
		printf_P(PSTR("%f\n"),controller());
}

void cmd_set_ref(int argc, char* argv[])
{
		set_ref(atof(argv[1]));
}

void cmd_update_states(int argc, char* argv[])
{
		update_states(atof(argv[1]),atof(argv[2]),atof(argv[3]));
}

void cmd_update_state1(int argc, char* argv[])
{
	update_state1(atof(argv[1]));
}

void cmd_update_state2(int argc, char* argv[])
{
	update_state2(atof(argv[1]));
}

void cmd_update_state3(int argc, char* argv[])
{
	update_state3(atof(argv[1]));
}

void cmd_update_ctrl(int argc, char* argv[])
{
		update_ctrl(atof(argv[1]),atof(argv[2]),atof(argv[3]),atof(argv[4]));
}

void cmd_pil(int argc, char* argv[])
{
		printf_P(PSTR("%d\n"),pil());
}

void cmd_kal(int argc, char* argv[])
{
	kalman_state val = kalman_values();
	printf_P(PSTR("%lf,%lf\n"),val.x_angle,val.x_rate);
}

void cmd_balance(int argc, char* argv[])
{
	nsamples = atoi(argv[1]);
	enc_reset();
	kalman_reset();
	count = 0;

	event_set(event_balance);
	event_start();
}

void event_balance(void)
{
	float tau;
	uint8_t pwm;
	if(nsamples){
		tau = controller();
		pwm = control_allocation(tau);
		set_speed1(-pwm);
		set_speed2(pwm);
		nsamples--;
	}else{
		set_speed1(0);
		set_speed2(0);
		event_stop();
	}
}
