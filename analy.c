/*
 * analy.c
 *
 *  Created on: 2016��7��11��
 *      Author: ����ʤ
 *********************************************************************************************************
 ** �������� ��Communication_Decode()
 ** �������� ���������
 ** ��ڲ��� ����
 ** ���ڲ��� ����
 *********************************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "analy.h"
#include "config.h"
void Communication_Decode() {
	printf("Communication_Decode\n");
	MOTOR_INIT
	;
	MOTOR_MODE
	;
	if (buffer[0] == 0x00) {
		switch (buffer[1]) /*�������*/
		{
		case 0x01:
			MOTOR_FORWARD
			;
			return;
		case 0x02:
			MOTOR_BACK
			;
			return;
		case 0x03:
			MOTOR_LEFT
			;
			return;
		case 0x04:
			MOTOR_RIGHT
			;
			return;
		case 0x00:
			MOTOR_STOP
			;
			return;
		default:
			return;
		}
	} else if (buffer[0] == 0x01)/*�������*/
	{
		if (buffer[2] > 180)
			return;
		switch (buffer[1]) {
		//  case 0x07:angle1 = buffer[2];servo1.write(angle1);return;
		//  case 0x08:angle2 = buffer[2];servo2.write(angle2);return;
		default:
			return;
		}
	} else if (buffer[0] == 0x32) {
		return;
	}

}
/*
 *********************************************************************************************************
 ** �������� ��Communication_Decode()
 ** �������� ����ȡ����
 ** ��ڲ��� ���������ݱ�־λ��������������
 ** ���ڲ��� ����
 *********************************************************************************************************
 */
void Get_uartdata(int flag, unsigned char recv[]) {
	printf("Get_uartdata\n");
	static int i;
	acc_flag = 1;
	rec_flag = 1;

	if (flag != -1) {
		printf("rec_flag\n");
		while (rec_flag) {
			if (acc_flag == 1) {
				printf("acc_flag\n");
				if (recv[0] == 0xff) {
					printf("%x\n", recv[0]);
					rec_flag = 1;
					i = 0;
					acc_flag = 0;
				}
			} else {
				if (recv[i + 1] == 0xff) {
					rec_flag = 0;
					if (i == 3) {
						Communication_Decode();
					}
					i = 0;
				} else {
					buffer[i] = recv[i + 1];
					printf("buffer%x\n", buffer[i]);
					i++;
				}
			}
		}
	}
}

