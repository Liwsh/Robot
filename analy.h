/*
 * analy.h
 *
 *  Created on: 2016年7月11日
 *      Author: 李文胜
 *
 */

#ifndef ANALY_H_
#define ANALY_H_

#include "type.h"
#include "socket.h"
int rec_flag; /*读取数据状态标志位*/
int acc_flag; /*抛弃数据包头标志位*/
void Communication_Decode();
void Get_uartdata(int flag, unsigned char recv[]);

#endif /* ANALY_H_ */
