/*
 * decode.c
 *
 *  Created on: 2016年7月11日
 *      Author: 李文胜
 */
/*
 * 函数名称 ：Communication_Decode()
 * 函数功能 ：命令解码
 */
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "move.h"
#include "servo.h"
#include "camera.h"
#include "udpcap.h"
int Communication_Decode() {
//	printf("Communication_Decode\n");
	if (opbuffer[0] == 0x00) {
		if(opbuffer[1]==0x01)
			move_flag=1;
		else
			move_flag=0;
		switch (opbuffer[1])   //电机命令
		{
		case 0x01:
			printf("Straight line\n");
			move(1);
			break;
		case 0x02:
			printf("Back off\n");
			move(2);
			break;
		case 0x03:
			printf("Turn left\n");
			move(3);
			break;
		case 0x04:
			printf("Turn right\n");
			move(4);
			break;
		case 0x00:
			printf("Stop \n");
			move(0);
			break;
		default:
			break;
		}
	} else if (opbuffer[0] == 0x01)   //舵机命令
			{
		if (opbuffer[2] > 180)
			return 2;
		switch (opbuffer[1]) {
		  case 0x03:printf("servo vertical anlge:%x\n",opbuffer[2]);servo(opbuffer[2],3); break;
		  case 0x09:printf("servo parallel anlge:%x\n",opbuffer[2]);servo(opbuffer[2],9); break;
		default:
			return 2;
		}
	} else if (opbuffer[0] == 0x02)
			{
		switch (opbuffer[1])
				{
				case 0x01:
					printf("Take a picture\n");
					camera();
					break;
				case 0x02:
					printf("Upload pictures\n");
					udpcap();
					break;
				case 0x03:
					printf("Position recognition down\n");
					mpu_flag=0;
					break;
				case 0x04:
					printf("Position recognition \n");
					mpu_flag=1;
					break;
				case 0x00:
					break;
				default:
					break;
				}
	}
	return move_flag;
}
/*
 ** 函数名称 ：Communication_Decode()
 ** 函数功能 ：读取命令
 ** 参数 ：接受数据标志位，接收数据数组
 */
int Get_uartdata(int flag, unsigned char recv[]) {
	static int i;
	acc_flag = 1;
	rec_flag = 1;

	if (flag == 1) //判断缓冲器是否有数据装入
			{
		while (rec_flag) {
			if (acc_flag == 1) {
				if (recv[0] == 0xff) {
					rec_flag = 1;
					i = 0;
					acc_flag = 0;
				}
			} else {
				if (recv[i + 1] == 0xff) {
					rec_flag = 0;
					if (i == 3) {
						Communication_Decode();
						return 0;
					}
					i = 0;
				} else {
					opbuffer[i] = recv[i + 1];
					i++;
				}
			}
		}
	}
	return 0;
}

