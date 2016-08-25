/*
 * sensor.h
 *
 *  Created on: 2016年8月3日
 *      Author: 李文胜
 */
#ifndef SENSOR_H_
#define SENSOR_H_
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/*
 * 配置电子罗盘参数
 */
#define MPU6050_DEFAULT_ADDRESS 0x68
#define MPU6050_RA_PWR_MGMT_1	0x6B
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_GYRO_CONFIG	0x1B
#define MPU6050_RA_ACCEL_CONFIG 0x1C
#define MPU6050_CLOCK_PLL_XGYRO 0x01
#define MPU6050_GYRO_FS_250   	0x00
#define MPU6050_ACCEL_FS_2		0x00
#define PI 3.1415926535897932384626433832795
#define HMC5883L_I2C_ADDR 0x1E
#define HMC5883L_CONF_REG_A 0x00
#define HMC5883L_CONF_REG_B 0x01
#define HMC5883L_MODE_REG 0x02
#define HMC5883L_CONT_MODE 0x00
#define HMC5883L_DATA_REG 0x03
#define GA_1_3_REG 0x20
#define SCALE_1_22_MG 1.22

#define MAX_WAIT 10000

/*
 * 超声波距离，上次测量和当前测量状态值
 */
int distence;
int inp, pre;
/*
 * 电子罗盘参数
 */
unsigned char devAddr;
int16_t m_coor[3];
float m_declination;
float changle;
int angflag;
int angle;
unsigned char  h_buf[6];
/*
 * 互斥锁和同步条件
 */
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_mutex_t mutex1;
pthread_cond_t cond1;
float roll,pitch,yaw;
int16_t ax, ay, az, gx, gy, gz; //陀螺仪原始数据 3个加速度+3个角速度
unsigned char mpubuffer[14];
int irleft();
int irright();
int *ultrasonic();
int *warning(void *threadid);
int *hmc(void *threadid);
#endif /* SENSOR_H_ */
