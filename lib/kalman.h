#ifndef _KALMAN_H
#define _KALMAN_H

typedef struct {
	double T;
	double R_angle;
	double R_gyro;
	double Q_angle;
	double Q_rate;
	double Q_bias;
} kalman_param;

typedef struct {
	double x_angle;
	double x_rate;
	double x_bias;
	double P[3][3];
} kalman_state;

void kalman_init(void);
void kalman_reset(void);
void kalman_filter(void);
void kalman_tune(double,double,double,double,double);
kalman_state kalman_values(void);

#endif
