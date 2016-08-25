/*
 * mpu.h
 *
 *  Created on: 2016年8月11日
 *      Author: Administrator
 */

#ifndef MPU_H_
#define MPU_H_

#define MPU6050_DEFAULT_ADDRESS 0x68
#define MPU6050_RA_PWR_MGMT_1	0x6B
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_GYRO_CONFIG	0x1B
#define MPU6050_RA_ACCEL_CONFIG 0x1C
#define MPU6050_CLOCK_PLL_XGYRO 0x01
#define MPU6050_GYRO_FS_250   	0x00
#define MPU6050_ACCEL_FS_2		0x00
#define PI 3.1415926535897932384626433832795
uint8_t devAddr;
#define EVENT_NONE 0
#define EVENT_EVERY 1
#define EVENT_OSCILLATE 2
#define MAX_NUMBER_OF_EVENTS 10
#define NO_TIMER_AVAILABLE   -1

float timeChange = 20; //滤波法采样时间间隔毫秒
float dt = 0.02; //注意：dt的取值为滤波器采样时间
// 陀螺仪
float angleAx, gyroGy; //计算后的角度（与x轴夹角）和角速度
float roll,pitch,yaw;
int16_t ax, ay, az, gx, gy, gz; //陀螺仪原始数据 3个加速度+3个角速度
//一阶滤波
float x1, x2, y; //运算中间变量
//float dt=20*0.001;//注意：dt的取值为滤波器采样时间
float angle2; //er阶滤波角度输出
//卡尔曼滤波参数与函数
//float dt=20*0.001;//注意：dt的取值为kalman滤波器采样时间
//一下为运算中间变量
uint8_t buffer[14];
mraa_i2c_context i2c;

float P[2][2] = { { 1, 0 }, { 0, 1 } };
float Pdot[4] = { 0, 0, 0, 0 };
float Q_angle = 0.001, Q_gyro = 0.005; //角度数据置信度,角速度数据置信度
float R_angle = 0.5, C_0 = 1;
float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;

void timeupdate(void);
void update(unsigned long now);
void costupdate(unsigned long now, int i);
int findIndex(void);
int everyth(unsigned long period, void (*callback)(), int repeatCount);

void yijielvbo(float angle_m, float gyro_m);
void erjielvbo(float angle_m, float gyro_m);
float kalman_Filter(double angle_m, double gyro_m);
void getangle();
void print();

void mpu();
#endif /* MPU_H_ */
