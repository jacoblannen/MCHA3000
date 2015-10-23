#ifndef _CMD_H
#define _CMD_H

typedef struct
{
	const char * cmd;
	void (*func)(int argc, char *argv[]);
	const char * help;
} CMD_T;

char string_parser(char* inp, char* array_of_words[]);
int arg_count;
int cmd_flag;
char cmd_string[32];
char* out_array[50];

void do_cmd(char* cmd_str);

void cmd_help(int argc, char *argv[]);
void cmd_enc1_read(int argc, char *argv[]);
void cmd_enc2_read(int argc, char *argv[]);
void cmd_enc_reset(int argc, char *argv[]);
void cmd_pinout(int argc, char *argv[]);

void cmd_getctrl(int argc, char *argv[]);

void cmd_set_speed(int argc, char* argv[]);

void cmd_set_dir1(int argc, char* argv[]);
void cmd_set_speed1(int argc, char* argv[]);

void cmd_set_dir2(int argc, char* argv[]);
void cmd_set_speed2(int argc, char* argv[]);

void cmd_imu(int argc, char* argv[]);
void cmd_imu_log(int argc, char* argv[]);

void event_imu_log(void);

void cmd_enc_log(int argc, char* argv[]);
void event_enc_log(void);

void cmd_adc0_log(int argc, char* argv[]);
void event_adc0_log(void);

void cmd_motor_log(int argc, char* argv[]);
void event_motor_log(void);

void cmd_kal(int argc, char* argv[]);
void cmd_kalman_tune(int argc, char* argv[]);
void cmd_kalman_log(int argc, char* argv[]);
void event_kalman_log(void);

void cmd_controller(int argc, char* argv[]);
void cmd_set_ref(int argc, char* argv[]);
void cmd_update_states(int argc, char* argv[]);
void cmd_update_state1(int argc, char* argv[]);
void cmd_update_state2(int argc, char* argv[]);
void cmd_update_state3(int argc, char* argv[]);
void cmd_update_ctrl(int argc, char* argv[]);

void cmd_pil(int argc, char* argv[]);

void cmd_balance(int argc, char* argv[]);
void event_balance(void);

extern const CMD_T cmd_table[];

extern int log_count;

#endif
