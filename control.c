/*
 * control.c
 *
 *  Created on: 2016-7
 *      Author: Administrator
 */
#include <stdio.h>
#include <math.h>
#include "control.h"
#include "camera.h"
#include "move.h"
#include "sensor.h"
int calangle(){
	int pflag = 1;
	while (1) {
		pthread_mutex_lock(&mutex1);
		if (pflag == 1) {
			preangle = angle;
			pflag = 0;
		}
		nowangle = angle;
		pthread_mutex_unlock(&mutex1);
		cangle = abs(nowangle - preangle);
		if(cangle>80)
			break;
	}
	return cangle;
}
int judge(){
	pthread_mutex_lock(&mutex);
	if(distence<20)
	{
		lflag = irleft();
		rflag = irright();
		if (!lflag && !rflag)
			return 4;
		if (!lflag && rflag)
			return 4;
		if (lflag && !rflag)
			return 3;
		if (lflag && rflag)
			return 4;
	}
	else{
		lflag = irleft();
		rflag = irright();
		if (!lflag && !rflag)
			return 1;
		if (!lflag & rflag)
			return 1;
		if (lflag && !rflag)
			return 4;
		if (lflag && rflag)
			return 1;
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}
int sport(){
	mov = judge();
	move(mov);
	if (mov == 1)
		return angle;
	else if (mov == 3) {
//		if (calangle() > 80)
//			move(1);
		move(3);
		usleep(300000);
		move(0);
	} else {
//		if (calangle() > 80)
//			move(1);
		move(4);
				usleep(300000);
				move(0);
	}
	return cangle;
}
void control(){
	int fflag;
	printf("This is a Autonomous Control System\n");
	sleep(1);
	Node *head,*pnode,*nnode,*cnode;
	head = (Node*)malloc(sizeof(Node));

	while(1){
		sport();
		usleep(10000);
//	nnode->data.dir=judge();
//	nnode->data.angle=1;
//	nnode->data.tangle=1;
//	nnode->prior=pnode;
//	pnode->next=nnode;
//	pnode=nnode;
	}
}



