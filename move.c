/*
 * move.c
 *
 *  Created on: 2016Äê7ÔÂ7ÈÕ
 *      Author: Administrator
 */
#include <stdio.h>
#include "move.h"
#include "sensor.h"
void move(int move) {
	int flag;
	MOTOR_INIT;
	MOTOR_EN;
	MOTOR_MODE;
	switch (move) {
	case 1:
		MOTOR_FORWARD;
		break;
	case 2:
		MOTOR_BACK;
		break;
	case 3:
		MOTOR_LEFT;
		break;
	case 4:
		MOTOR_RIGHT;
		break;
	case 0:
		MOTOR_STOP;
		break;
	default:
		MOTOR_STOP;
		break;
	}
}

