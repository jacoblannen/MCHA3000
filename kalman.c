#include <stdio.h>
#include <avr/io.h>
#include "kalman.h"
#include <math.h>
#include <avr/pgmspace.h> 
#include "imu.h"

static kalman_param parameters;
static kalman_state state;

void kalman_init(void)
{
	parameters.T = 1.0/100.0;
	parameters.R_angle = 10.0;
	parameters.R_gyro = 0.01;
	parameters.Q_angle = 0.001;
	parameters.Q_rate = 0.005;
	parameters.Q_bias = 0.00001;

	state.x_angle = 0;
	state.x_rate = 0;
	state.x_bias = 0;
	state.P[0][0] = 1;
	state.P[0][1] = 0;
	state.P[0][2] = 0;
	state.P[1][0] = 0;
	state.P[1][1] = 1;
	state.P[1][2] = 0;
	state.P[2][0] = 0;
	state.P[2][1] = 0;
	state.P[2][2] = 1;
}

void kalman_reset(void)
{
	state.x_angle = 0;
	state.x_rate = 0;
	state.x_bias = 0;
	state.P[0][0] = 1;
	state.P[0][1] = 0;
	state.P[0][2] = 0;
	state.P[1][0] = 0;
	state.P[1][1] = 1;
	state.P[1][2] = 0;
	state.P[2][0] = 0;
	state.P[2][1] = 0;
	state.P[2][2] = 1;
}

void kalman_filter(void)
{
	imu_val data = imu_read();

	double x_gyro = ((double)data.gyrox/131)*M_PI/180.0;
	double x_angle = atan2((double)data.accz,(double)-data.accy);

	//Covariance prediction
	// P = A*P*A' + Q
	state.P[0][0] += (state.P[0][1] + state.P[1][0] + state.P[1][1]*parameters.T)*parameters.T + parameters.Q_angle;
	state.P[1][1] += parameters.Q_rate;
	state.P[2][2] += parameters.Q_bias;
	state.P[0][1] += state.P[1][1]*parameters.T;
	state.P[0][2] += state.P[1][2]*parameters.T;
	state.P[1][0] += state.P[1][1]*parameters.T;
	state.P[2][0] += state.P[2][1]*parameters.T;

	// State prediction
	// x = A*x
	state.x_angle += state.x_rate*parameters.T;

	// Measurement update due to accelerometer data
	// Kalman gain
	double K[3];
	double K_denom = state.P[0][0] + parameters.R_angle;
	K[0] = state.P[0][0] / K_denom;
	K[1] = state.P[1][0] / K_denom;
	K[2] = state.P[2][0] / K_denom;

	// State correction
	double x_err = x_angle - state.x_angle;
	state.x_angle += K[0]*x_err;
	state.x_rate += K[1]*x_err;
	state.x_bias += K[2]*x_err;

	// Covariance update
	// P = (I - K*C)*P
	state.P[0][0] -= K[0]*state.P[0][0];
	state.P[0][1] -= K[0]*state.P[0][1];
	state.P[0][2] -= K[0]*state.P[0][2];
	state.P[1][0] -= K[1]*state.P[0][0];
	state.P[1][1] -= K[1]*state.P[0][1];
	state.P[1][2] -= K[1]*state.P[0][2];
	state.P[2][0] -= K[2]*state.P[0][0];
	state.P[2][1] -= K[2]*state.P[0][1];
	state.P[2][2] -= K[2]*state.P[0][2];

	// Measurement update due to gyro data
	// Kalman gain
	K_denom = state.P[1][1] + state.P[1][2] + state.P[2][1] + state.P[2][2] + parameters.R_gyro;
	K[0] = (state.P[0][1] + state.P[0][2]) / K_denom;
	K[1] = (state.P[1][1] + state.P[1][2]) / K_denom;
	K[2] = (state.P[2][1] + state.P[2][2]) / K_denom;

	// State correction
	x_err = x_gyro - state.x_rate - state.x_bias;
	state.x_angle += K[0]*x_err;
	state.x_rate += K[1]*x_err;
	state.x_bias += K[2]*x_err;

	// Covariance update
	// P = (I - K*C)*P
	state.P[0][0] -= K[0]*(state.P[1][0] + state.P[2][0]);
	state.P[0][1] -= K[0]*(state.P[1][1] + state.P[2][1]);
	state.P[0][2] -= K[0]*(state.P[1][2] + state.P[2][2]);
	state.P[1][0] -= K[1]*(state.P[1][0] + state.P[2][0]);
	state.P[1][1] -= K[1]*(state.P[1][1] + state.P[2][1]);
	state.P[1][2] -= K[1]*(state.P[1][2] + state.P[2][2]);
	state.P[2][0] -= K[2]*(state.P[1][0] + state.P[2][0]);
	state.P[2][1] -= K[2]*(state.P[1][1] + state.P[2][1]);
	state.P[2][2] -= K[2]*(state.P[1][2] + state.P[2][2]);
}

void kalman_tune(double R_angle,double R_gyro,double Q_angle,double Q_rate,double Q_bias)
{
	parameters.R_angle = R_angle;
	parameters.R_gyro = R_gyro;
	parameters.Q_angle = Q_angle;
	parameters.Q_rate = Q_rate;
	parameters.Q_bias = Q_bias;

}

kalman_state kalman_values(void)
{
	kalman_filter();
	return(state);
}
