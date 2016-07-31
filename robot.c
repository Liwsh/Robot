/*
 * main.c
 *  Created on: 2016年7月7日
 *      Author: 李文胜
 *     	Contact: 649542997@qq.com
 */

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "robot.h"

int main(int argc, char *argv[]) {
	pid_t pid;
	pthread_t th1, th2, th3, th4;
	void *son, *war, *hmc, *sock;
	int th1v, th2v, th3v, th4v;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_cond_init(&cond1, NULL);
	th1v = pthread_create(&th1, NULL, (void *) ultrasonic, NULL);
	if (th1v != 0) {
		printf("Create pthread error!\n");
		return 1;
	}
	th2v = pthread_create(&th2, NULL, (void *) buzzer, NULL);
	if (th2v != 0) {
		printf("Create pthread error!\n");
		return 1;
	}
	th3v = pthread_create(&th3, NULL, (void *) hmc, NULL);
	if (th3v != 0) {
		printf("Create pthread error!\n");
		return 1;
	}
	th4v = pthread_create(&th4, NULL, (void *) sock, NULL);
	if (th4v != 0) {
		printf("Create pthread error!\n");
		return 1;
	}
	printf("This is the main process.\n");
	pthread_join(th1, &son);
	pthread_join(th2, &war);
	pthread_join(th3, &hmc);
	pthread_join(th4, &sock);
	printf("The thread return value is%d\n", (int) son);
	return 0;
}
