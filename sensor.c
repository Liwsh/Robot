/*
 * sensor.c
 *
 *  Created on: 2016年8月3日
 *      Author: 李文胜
 */
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <mraa.h>
#include "sensor.h"
#include "decode.h"
#include "sock.h"
#include "move.h"
#include "servo.h"
float direction(void) {
	return atan2(m_coor[1] * SCALE_1_22_MG, m_coor[0] * SCALE_1_22_MG)
			+ m_declination;
}
void set_declination(float dec) {
	m_declination = dec;
}
int irleft() {
	mraa_gpio_context ir = mraa_gpio_init(14);
	int i;
	mraa_gpio_dir(ir, MRAA_GPIO_IN);
	i = mraa_gpio_read(ir);
	/*i值为1时，无障碍，为0有障碍*/
	return i;
}
int irright() {
	mraa_gpio_context ir = mraa_gpio_init(15);
	int i;
	mraa_gpio_dir(ir, MRAA_GPIO_IN);
	i = mraa_gpio_read(ir);
	return i;
}
/*
 * 函数名称 ：uarlsonic()
 * 函数功能 ：距离检测函数
 * 内容描述 ：声音在空气中的传播速度约为34,300厘米/秒。34,300除以1,000,000厘米/微秒。
 * 即为：0.0343厘米/微秒，1/0.0343 微秒/厘米即：29.15 微秒/厘米。s=v*t(路程=速度X时间),
 * 所以速度v=s/t=1/29,s是来回2倍的s所测距离，所以v=s/2/t=s/2t=1、2X29=1/58
 */
int *ultrasonic() {
	printf("ultrasonic thread\n");
	int i, flag;
	int usetime;
	unsigned char forward[5] = { 0xff, 0x00, 0x01, 0x00, 0xff };
	struct timeval tv1, tv2;
	struct timezone tv;
	mraa_gpio_context trig = mraa_gpio_init(11);
	mraa_gpio_context echo = mraa_gpio_init(2);
	mraa_gpio_dir(trig, MRAA_GPIO_OUT);
	mraa_gpio_dir(echo, MRAA_GPIO_IN);
	while (1) {
		/*Trig输入 10US以上的高电平，系统便发出 8 个 40KHZ的超声波脉冲，当检测到回波信号后，通过 Echo管脚输出*/
//		mraa_gpio_write(trig, 0);
//		usleep(10);
		mraa_gpio_write(trig, 1);
		usleep(20);
		mraa_gpio_write(trig, 0);
		/*循环函数检测Echo的电平变化，由高变低，计算用时*/

		i = 0;
		while ((mraa_gpio_read(echo) == 0) && (i < MAX_WAIT)) {
			usleep(1);
			i++;
		}
		gettimeofday(&tv1, &tv);
		if (mraa_gpio_read(echo) == 0 || i == MAX_WAIT) {
			///		printf(" RET_ERROR\n");
		}
		i = 0;
		while ((mraa_gpio_read(echo) == 1) && (i < MAX_WAIT)) {
			usleep(1);
			i++;
		}
		gettimeofday(&tv2, &tv);
		if (mraa_gpio_read(echo) == 1 || i == MAX_WAIT) {
//			printf(" RET_ERROR\n");
		}

		usetime = (tv2.tv_sec - tv1.tv_sec) * pow(10, 6) + tv2.tv_usec
				- tv1.tv_usec;
		pthread_mutex_lock(&mutex);
		distence = usetime / 58;
		pthread_mutex_unlock(&mutex);

//		pthread_mutex_lock(&mutex);
		usleep(1000);
		if (move_flag) {
			if (distence < 18)
				inp = 1;
			else
				inp = 0;
		} else
			inp = 0;
		if (inp == 1)
			move(0);
		/*当距离小于安全距离时，触发warning线程报警*/
//			pthread_cond_signal(&cond);
//		pthread_mutex_unlock(&mutex);
//		printf("distence is %d cm\n", distence);
		usleep(10000);
	}

	return inp;
}
/*
 * 函数名称 ：warning()
 * 函数功能 ：危险信息报警
 */
int *warning(void *threadid) {
	struct timeval tv3, tv4;
	struct timezone tz5;
	int usetim;
	int flag = 1;
	devAddr = MPU6050_DEFAULT_ADDRESS;
	mraa_i2c_context i2c;
	i2c = mraa_i2c_init(0);
	mraa_i2c_address(i2c, devAddr);
	mpu_flag=0;
	while(1){
		if(mpu_flag){
	mraa_i2c_write_byte_data(i2c, MPU6050_CLOCK_PLL_XGYRO,
	MPU6050_RA_PWR_MGMT_1);
	mraa_i2c_write_byte_data(i2c, MPU6050_GYRO_FS_250, MPU6050_RA_GYRO_CONFIG);
	mraa_i2c_write_byte_data(i2c, MPU6050_ACCEL_FS_2, MPU6050_RA_ACCEL_CONFIG);

		mraa_i2c_read_bytes_data(i2c, MPU6050_RA_ACCEL_XOUT_H, mpubuffer, 14);
		ax = (((int16_t) mpubuffer[0]) << 8) | mpubuffer[1];
		ay = (((int16_t) mpubuffer[2]) << 8) | mpubuffer[3];
		az = (((int16_t) mpubuffer[4]) << 8) | mpubuffer[5];
		gx = (((int16_t) mpubuffer[8]) << 8) | mpubuffer[9];
		gy = (((int16_t) mpubuffer[10]) << 8) | mpubuffer[11];
		gz = (((int16_t) mpubuffer[12]) << 8) | mpubuffer[13];
		if (flag) {
			gettimeofday(&tv3, &tz5);
			flag = 0;
		}
		//    angleAx=atan2(ax,az)*180/PI;//计算与x轴夹角
		//    gyroGy=-gy/131.00;//计算角速度
		pitch = atan2(ax, az) * 180 / PI;			//计算与x轴夹角
		roll = atan2(ay, az) * 180 / PI; //计算与y轴夹角
		yaw = atan2(ax, ay) * 180 / PI; //计算与y轴夹角
		gettimeofday(&tv4, &tz5);
		usetim = (tv4.tv_sec - tv3.tv_sec)* pow(10, 6)
				+ (tv4.tv_usec - tv3.tv_usec);
		if (usetim > 400000) {
			printf("roll=%0.1f", (roll+1));
			printf("      yaw=%0.1f ", yaw);
			printf("      pitch=%0.1f \n", (pitch-4.0));
			flag = 1;
		}
		}
	}
	return 1;
}
/*
 * 函数名称 ：hmc()
 * 函数功能 ：获取当前的方位
 */
int *hmc(void *threadid) {
	int bus = 0;
	float preangle, nowangle;
	mraa_i2c_context m_i2c;
	m_i2c = mraa_i2c_init(bus);
	mraa_i2c_address(m_i2c, HMC5883L_I2C_ADDR);
	if (mraa_i2c_read_bytes_data(m_i2c, 0x0A, h_buf, 3) == 3) {
		printf("%c%c%c\n", h_buf[0], h_buf[1], h_buf[2]);
	} else {
		printf("hmc create fail\n");
//		exit(1);
	}
	//		h_buf[0] = HMC5883L_CONF_REG_B;
	//		h_buf[1] = GA_1_3_REG;
	//		mraa_i2c_write(m_i2c, h_buf, 2);
	mraa_i2c_write_byte_data(m_i2c, 0x20, 0x01);

	//		h_buf[0] = HMC5883L_MODE_REG;
	//		h_buf[1] = HMC5883L_CONT_MODE;
	//		mraa_i2c_write(m_i2c, h_buf, 2);
	mraa_i2c_write_byte_data(m_i2c, 0x00, 0x02);
	set_declination(1.16); // Set your declination from true north in radians
	while (1) {
		mraa_i2c_write_byte(m_i2c, HMC5883L_DATA_REG);
		mraa_i2c_read(m_i2c, h_buf, 6);
		/*获取三维空间的x轴参数*/
		m_coor[0] = (h_buf[0] << 8) | h_buf[1];
		/*获取三维空间的z轴参数*/
		m_coor[2] = (h_buf[2] << 8) | h_buf[3];
		/*获取三维空间的y轴参数*/
		m_coor[1] = (h_buf[4] << 8) | h_buf[5];
		pthread_mutex_lock(&mutex1);
		angle = atan2(m_coor[1], m_coor[0]) * 180 / M_PI + 180;
		pthread_mutex_unlock(&mutex1);
//		fprintf(stdout, "coor: %5d %5d %5d ", m_coor[0], m_coor[1], m_coor[2]);
//		fprintf(stdout, "heading: %d  \n", angle);
		usleep(1000);
	}
	return 1;
}

