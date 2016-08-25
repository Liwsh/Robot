/*
 * car.c
 *
 *  Created on: 2016年8月3日
 *      Author: 李文胜
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sensor.h"
#include "move.h"
#include "servo.h"
#include "sock.h"
//#include "mpu.h"
/*
 * 创建多线程，包括传感器线程，警告线程，网络线程
 */
int main() {
	pthread_t th1, th2, th3, th4,th5;
	void *son,*hm, *soc,*serv;
	int th1v, th2v, th3v, th4v,th5v;
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&smutex, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_cond_init(&scond, NULL);
	pthread_cond_init(&cond1, NULL);
	printf("This is the main process.\n");
	sleep(2);
	th1v = pthread_create(&th1, NULL, (void *) sock, NULL);
	if (th1v != 0) {
		printf("Create sock pthread error!\n");
		return 1;
	}
	th2v = pthread_create(&th2, NULL, (void *) ultrasonic, NULL);
	if (th2v != 0) {
		printf("Create ultrasonic pthread error!\n");
		return 1;
	}
	th3v = pthread_create(&th3, NULL, (void *) warning, NULL);
	if (th3v != 0) {
		printf("Create mpuning pthread error!\n");
		return 1;
	}
//	th4v = pthread_create(&th4, NULL, (void *) hmc, NULL);
//	if (th4v != 0) {
//		printf("Create hmc pthread error!\n");
//		return 1;
////	}
	/*
	 * 等待线程结束
	 */
	pthread_join(th1, &soc);
	pthread_join(th2, &son);
	pthread_join(th3, &warning);
//	pthread_join(th4, &hm);
	return 0;
}
