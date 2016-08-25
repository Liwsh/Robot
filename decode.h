/*
 * decode.h
 *
 *  Created on: 2016年8月3日
 *      Author: Administrator
 */

#ifndef DECODE_H_
#define DECODE_H_

#include "sock.h"

int rec_flag; /*读取数据状态标志位*/
int acc_flag; /*抛弃数据包头标志位*/
int move_flag;
int mpu_flag;

unsigned char opbuffer[3];	/*数据接收数组*/

int Communication_Decode();
int Get_uartdata(int flag, unsigned char recv[]);


#endif /* DECODE_H_ */
