/*
 * analy.h
 *
 *  Created on: 2016��7��11��
 *      Author: ����ʤ
 *
 */

#ifndef ANALY_H_
#define ANALY_H_

#include "type.h"
#include "socket.h"
int rec_flag; /*��ȡ����״̬��־λ*/
int acc_flag; /*�������ݰ�ͷ��־λ*/
void Communication_Decode();
void Get_uartdata(int flag, unsigned char recv[]);

#endif /* ANALY_H_ */
