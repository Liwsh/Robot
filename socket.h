/*
 * socket.h
 *
 *  Created on: 2016Äê7ÔÂ8ÈÕ
 *      Author: Liwensheng
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include<errno.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/un.h>
#include<sys/ioctl.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define SERVPORT 2001
#define BACKLOG 20
#define MAX_CON 10
#define MAX_DATA 5

unsigned char sendBuf[MAX_DATA], recvBuf[MAX_DATA];

int sock();
#endif /* SOCKET_H_ */
