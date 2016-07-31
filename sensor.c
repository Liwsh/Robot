/*
 * sensor.c
 *
 *  Created on: 2016年7月14日
 *      Author: Administrator
 */
/*
 * thread.c
 *
 *  Created on: 2016年7月13日
 *      Author: 李文胜
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include "sensor.h"
int single = 0;
float distence;
float direction(void) {
	return atan2(m_coor[1] * SCALE_0_92_MG, m_coor[0] * SCALE_0_92_MG)
			+ m_declination;
}

float heading(void) {
	float dir = direction() * 180 / M_PI;
	if (dir < 0) {
		dir += 360.0;
	}
	return dir;
}

float get_declination() {
	return m_declination;
}
void *ultrasonic(void *threadid) {
	int flag;
	int voltage, usetime;
	struct timeval tv1, tv2;
	struct timezone tv;
	mraa_gpio_context trig = mraa_gpio_init(2);
	mraa_gpio_context echo = mraa_gpio_init(3);
	mraa_gpio_dir(trig, MRAA_GPIO_OUT);
	mraa_gpio_dir(echo, MRAA_GPIO_IN);
	while (1) {
		usleep(100000);
		mraa_gpio_write(trig, 0);
		usleep(2);
		mraa_gpio_write(trig, 1);
		usleep(10);
		mraa_gpio_write(trig, 0);

		flag = 1;
		while (flag) {
			voltage = mraa_gpio_read(echo);
			if (voltage == 1) {
				flag = 0;
				gettimeofday(&tv1, &tv);
			}
		}

		//		printf("begin voltage %d\n", voltage);
		flag = 1;
		while (flag) {
			voltage = mraa_gpio_read(echo);
			if (voltage == 0) {
				flag = 0;
				gettimeofday(&tv2, &tv);
			}
		}

		usetime = (tv2.tv_sec - tv1.tv_sec) * pow(10, 6) + tv2.tv_usec
				- tv1.tv_usec;
		distence = usetime / 58.00;

		pthread_mutex_lock(&mutex);
		pre = inp;
		inp = (distence >= 0 && distence <= 10) ? 1 : 0;
		//	printf("%d\n", inp);
		pthread_mutex_unlock(&mutex);
		if (inp == 1)
			pthread_cond_signal(&cond);
		//		printf("distence is %0.2f cm\n", distence);

	}
	return ((void *) 1);
}

void *buzzer(void *threadid) {
	int i;
	mraa_gpio_context buzzer = mraa_gpio_init(4);
	mraa_gpio_context led = mraa_gpio_init(13);
	mraa_gpio_dir(buzzer, MRAA_GPIO_OUT);
	mraa_gpio_dir(led, MRAA_GPIO_OUT);
	MOTOR_INIT
	;
	MOTOR_MODE
	;
	while (1) {
		pthread_mutex_lock(&mutex);
		while (inp != 1)
			pthread_cond_wait(&cond, &mutex);
		if (inp == 1) {
			MOTOR_STOP
			;
		} else {
			MOTOR_FORWARD
			;
		}
		mraa_gpio_write(led, inp);
		//		if(inp==1){
		//		 for(i=0;i<5;i++)//输出一个频率的声音
		//			    {
		//				 mraa_gpio_write(buzzer,1);//发声音
		//			     usleep(500);//延时1ms
		//			     mraa_gpio_write(buzzer,0);//不发声音
		//			     usleep(500);//延时ms
		//			    }
		//			   for(i=0;i<100;i++)//输出另一个频率的声音,这里的100与前面的80一样，用来控制频率，可以自己调节
		//			     {
		//				   mraa_gpio_write(buzzer,1);
		//				   usleep(2000);;
		//			      mraa_gpio_write(buzzer,0);
		//			      usleep(2000);;
		//			        }
		//		}else
		//		{
		//			printf("distence safe\n");
		//		}
		pthread_mutex_unlock(&mutex);
	}

	return ((void *) 2);
}
void *hmc(void *threadid) {
	int usetime, flag = 1;
	int bus = 0;
	float preangle, nowangle, changle, addangle;
	int16_t *pos;
	struct timeval tv1, tv2;
	struct timezone tz;
	mraa_i2c_context m_i2c;
	m_i2c = mraa_i2c_init(bus);

	mraa_i2c_address(m_i2c, HMC5883L_I2C_ADDR);
	m_rx_tx_buf[0] = HMC5883L_CONF_REG_B;
	m_rx_tx_buf[1] = GA_1_3_REG;
	mraa_i2c_write(m_i2c, m_rx_tx_buf, 2);

	mraa_i2c_address(m_i2c, HMC5883L_I2C_ADDR);
	m_rx_tx_buf[0] = HMC5883L_MODE_REG;
	m_rx_tx_buf[1] = HMC5883L_CONT_MODE;
	mraa_i2c_write(m_i2c, m_rx_tx_buf, 2);

	set_declination(1.16); /*设置所在地区的磁偏角弧度*/

	while (1) {
		gettimeofday(&tv1, &tz);
		mraa_i2c_write_byte(m_i2c, HMC5883L_DATA_REG);

		mraa_i2c_read(m_i2c, m_rx_tx_buf, DATA_REG_SIZE);

		m_coor[0] = (m_rx_tx_buf[HMC5883L_X_MSB_REG] << 8)
				| m_rx_tx_buf[HMC5883L_X_LSB_REG];
		// z
		m_coor[2] = (m_rx_tx_buf[HMC5883L_Z_MSB_REG] << 8)
				| m_rx_tx_buf[HMC5883L_Z_LSB_REG];
		// y
		m_coor[1] = (m_rx_tx_buf[HMC5883L_Y_MSB_REG] << 8)
				| m_rx_tx_buf[HMC5883L_Y_LSB_REG];
		float angle = atan2(m_coor[1], m_coor[0]) * 180 / M_PI + 180;
		pthread_mutex_lock(&mutex1);
		nowangle = angle;
		if (angch_flag == 1 || angch_flag == 2 || angch_flag == 5) {
			printf("angle:%0.2f\n", angle);
			flag = 1;
		}
		//left
		if (flag == 1)
			preangle = angle;
		if (angch_flag == 3) {
			if (preangle > nowangle)
				changle = preangle - nowangle;
			else
				changle = 360 + preangle - nowangle;
			printf("preangle:%0.2f  changle:%0.2f\n", preangle, changle);
		}
		//rigth
		if (angch_flag == 4) {
			if (preangle > nowangle)
				changle = preangle - nowangle - 360;
			else
				changle = preangle - nowangle;
			printf("preangle:%0.2f  changle:%0.2f\n", preangle, changle);
		}
		flag++;
		pthread_mutex_unlock(&mutex1);
//		while(flag){
//		gettimeofday(&tv2,&tz);
//		usetime =(tv2.tv_sec-tv1.tv_sec)*pow(10,6)+ tv2.tv_usec - tv1.tv_usec;
//			if(usetime>100){
//				flag=0;
//			}
//		}
//		fprintf(stdout, "coor: %5d %5d %5d ", pos[0], pos[1], pos[2]);
//		fprintf(stdout, "heading: %5.2f direction: %3.2f\n", heading(), angle);
	}

	return ((void *) 3);
}
