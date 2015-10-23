#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h> 

#include "i2cmaster.h"
#include "imu.h"

void imu_init(void)
{
	uint8_t ret;
	
	ret = i2c_start(MPU6050_ADDRESS+I2C_WRITE);
	if(ret){
		i2c_stop();
		printf_P(PSTR("I2C failed to find IMU\n"));
	}else{
		printf_P(PSTR("IMU found\n"));
		i2c_write(MPU6050_RA_PWR_MGMT_1);
		i2c_write(0);
		i2c_stop();
	}
}

imu_val imu_read(void)
{
	imu_dat result;
	imu_val values;

	i2c_start_wait(MPU6050_ADDRESS+I2C_WRITE);
	i2c_write(MPU6050_RA_ACCEL_XOUT_H);
	i2c_rep_start(MPU6050_ADDRESS+I2C_READ);
	result.accx_h = i2c_readAck();
	result.accx_l = i2c_readAck();
	result.accy_h = i2c_readAck();
	result.accy_l = i2c_readAck();
	result.accz_h = i2c_readAck();
	result.accz_l = i2c_readAck();
	result.temp_h = i2c_readAck();
	result.temp_l = i2c_readAck();
	result.gyrox_h = i2c_readAck();
	result.gyrox_l = i2c_readAck();
	result.gyroy_h = i2c_readAck();
	result.gyroy_l = i2c_readAck();
	result.gyroz_h = i2c_readAck();
	result.gyroz_l = i2c_readNak();
	i2c_stop();

	values.accx=result.accx_h;
	values.accx=values.accx<<8;
	values.accx+=result.accx_l;
	values.accy=result.accy_h;
	values.accy=values.accy<<8;
	values.accy+=result.accy_l;
	values.accz=result.accz_h;
	values.accz=values.accz<<8;
	values.accz+=result.accz_l;
	values.gyrox=result.gyrox_h;
	values.gyrox=values.gyrox<<8;
	values.gyrox+=result.gyrox_l;
	values.gyroy=result.gyroy_h;
	values.gyroy=values.gyroy<<8;
	values.gyroy+=result.gyroy_l;
	values.gyroz=result.gyroz_h;
	values.gyroz=values.gyroz<<8;
	values.gyroz+=result.gyroz_l;

	return(values);
}
