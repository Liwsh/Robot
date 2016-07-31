/*
 * sensor.h
 *
 *  Created on: 2016年7月14日
 *      Author: Administrator
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "mraa.h"

#define MAX_BUFFER_LENGTH 6
#define MAX_BUFFER_LENGTH 6
#define HMC5883L_I2C_ADDR 0x1E

//configuration registers
#define HMC5883L_CONF_REG_A 0x00
#define HMC5883L_CONF_REG_B 0x01

//mode register
#define HMC5883L_MODE_REG 0x02

//data register
#define HMC5883L_X_MSB_REG 0
#define HMC5883L_X_LSB_REG 1
#define HMC5883L_Z_MSB_REG 2
#define HMC5883L_Z_LSB_REG 3
#define HMC5883L_Y_MSB_REG 4
#define HMC5883L_Y_LSB_REG 5
#define DATA_REG_SIZE 6

//status register
#define HMC5883L_STATUS_REG 0x09

//ID registers
#define HMC5883L_ID_A_REG 0x0A
#define HMC5883L_ID_B_REG 0x0B
#define HMC5883L_ID_C_REG 0x0C

#define HMC5883L_CONT_MODE 0x00
#define HMC5883L_DATA_REG 0x03

//scales
#define GA_0_88_REG 0x00 << 5
#define GA_1_3_REG 0x01 << 5
#define GA_1_9_REG 0x02 << 5
#define GA_2_5_REG 0x03 << 5
#define GA_4_0_REG 0x04 << 5
#define GA_4_7_REG 0x05 << 5
#define GA_5_6_REG 0x06 << 5
#define GA_8_1_REG 0x07 << 5

//digital resolutions
#define SCALE_0_73_MG 0.73
#define SCALE_0_92_MG 0.92
#define SCALE_1_22_MG 1.22
#define SCALE_1_52_MG 1.52
#define SCALE_2_27_MG 2.27
#define SCALE_2_56_MG 2.56
#define SCALE_3_03_MG 3.03
#define SCALE_4_35_MG 4.35

int16_t m_coor[3];
float m_declination;
uint8_t m_rx_tx_buf[MAX_BUFFER_LENGTH];
int rec_flag;    //读取数据状态标志位
int acc_flag;	 //抛弃数据包头标志位
int ang_flag;
int angch_flag;
unsigned char buffer[3];	//数据接收数组

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_mutex_t mutex1;
pthread_cond_t cond1;
int inp = 1, pre = 0;
/*left为逆时针旋转，角度为正，right为顺时针旋转，角度为负*/
void *ultrasonic(void *threadid);
void *buzzer(void *threadid);
void *hmc(void *threadid);

#endif /* SENSOR_H_ */
