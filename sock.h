/*
 * socket.h
 *
 *  Created on: 2016Äê7ÔÂ8ÈÕ
 *      Author: Liwensheng
 */

#ifndef SOCK_H_
#define SOCK_H_

#include <mraa.h>
#include <pthread.h>
#define SERVPORT 2001
#define BACKLOG 20
#define MAX_CON 10
#define MAX_DATA 5
#define Rdata    1
#define Ndata	 -1

unsigned char sendBuf[MAX_DATA], recvBuf[MAX_DATA];
pthread_mutex_t smutex;
pthread_cond_t scond;
int TIMEOUT;
void *sock();

#endif /* SOCKET_H_ */
