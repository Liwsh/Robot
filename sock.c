/*
 * socket.c
 *
 *  Created on: 2016年7月10日
 *      Author: Liwensheng
 ** 函数名称 ：sock()
 ** 函数功能 ：数据接收
 */
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "sock.h"
#include "decode.h"
#include "servo.h"
#include "control.h"

//void *sock() {
//
//	int num = 15,usetime;
//	socklen_t sin_size;
//	int sockfd,cliefd, recvBytes, res;
//	struct timeval tv1, tv2;
//	struct timeval timeout,tv;
//	fd_set fdr, fdw, fde;
//	char ip[] = "192.168.1.116";
//	unsigned char forwad[5]={0xff,0x00,0x01,0x00,0xff};
//	/*该结构记录主机的信息，包括主机名、地址类型、地址长度等。*/
//	struct hostent *host;
//
//	/*定义结构体用来保存套接字信息*/
//	struct sockaddr_in servAddr,clieAddr;
//
//	/*这个函数的传入值是域名或者主机名。传出值，是一个hostent的结构。如果函数调用失败，将返回NULL。*/
////	if ((host = gethostbyname(ip)) == NULL) {
////		perror("fail to get host by name");
////		exit(1);
////	}
//	printf("Success to get host by name...\n");
//
//	/*建立一个socket连接，AF_INET采用IPv4协议，SOCK_STREAM采用流式socket即TCP*/
//	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//		perror("fail to establish a socket");
//		exit(1);
//	}
//	printf("Success to establish a socket...\n");
//
//	/*初始化结构体*/
//	servAddr.sin_family = AF_INET;
//	/*套接字使用的端口，htons是将整型变量从主机字节序转变成网络字节序，存储方式变为：高位字节存放在内存的低地址处*/
//	servAddr.sin_port = htons(SERVPORT);
//	/*表示的是主机的ip地址，注意，这个是以网络字节序存储的。*/
////	servAddr.sin_addr = *((struct in_addr *) host->h_addr);
//	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	/*未使用的字段，填充为0*/
//	bzero(&(servAddr.sin_zero), 8);
//
//	if(bind(sockfd,(struct sockaddr *)&servAddr,sizeof(struct sockaddr))==-1)
//		perror("bind");
//
//	if(listen(sockfd,BACKLOG)==-1)
//		perror("listen");
//	sin_size=sizeof(clieAddr);
//	while((cliefd=accept(sockfd,(struct sockaddr*)&clieAddr,&sin_size))==-1)
//		printf("not accept\n");
//	printf("accept success\n");
//
//	TIMEOUT = 10;
////	while (num--) {
////		/*connect()用来将参数sockfd 的socket 连至参数serv_Addr 指定的网络地址. */
////		if (connect(sockfd, (struct sockaddr *) &servAddr,
////				sizeof(struct sockaddr_in)) == -1) {
////			printf("Restart connect\n");
////			sleep(1);
////		} else
////			num = 0;
////	}
//
////	/*设置套接口非阻塞*/
////	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
////		perror("Set sockfd fail...\n");
////		close(sockfd);
////	}
//
//	//	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
//	//		perror("Set sockfd fail...\n");
//	//		close(sockfd);
//	//	}
//
//		if (fcntl(cliefd, F_SETFD, O_NONBLOCK) < 0) {
//			perror("Set sockfd fail...\n");
//			close(sockfd);
//		}
//	while (1) {
//		/*将select()监视的读、写、异常的文件描述符集合清除*/
//		FD_ZERO(&fdr);
//		FD_ZERO(&fdw);
//		FD_ZERO(&fde);
//
//		/*将标准输入文件描述符和新建描述符添加到select()监视的文件描述符集合中*/
//		FD_SET(cliefd, &fdr);
//		FD_SET(cliefd, &fdw);
//		FD_SET(cliefd, &fde);
//
//		/*select()在被监视窗口等待的时间*/
//		timeout.tv_sec = TIMEOUT;
//		timeout.tv_usec = 0;
//
//		res = select(cliefd + 1, &fdr, 0, 0, &timeout);
//		if (res < 0) {
//			perror("select error:\n");
//			close(sockfd);
//			break;
//		}
//		if (res == 0) {
//			continue;
//		}
//		if (FD_ISSET(cliefd, &fdr)) {
//
//			recvBytes = recv(cliefd, recvBuf, sizeof(recvBuf), 0);
//			if (recvBuf[0] == 0)
//			{
//				move_flag=0;
//				printf("no data\n");
////				sleep(1);
////				control();
//			}
//			else {
//				Get_uartdata(Rdata,recvBuf);
//				printf("Client1:%x\n", recvBuf[0]);
//				printf("Client2:%x\n", recvBuf[1]);
//				printf("Client3:%x\n", recvBuf[2]);
//				printf("Client4:%x\n", recvBuf[3]);
//				printf("Client5:%x\n", recvBuf[4]);
//			}
//			memset(recvBuf, 0x00, MAX_DATA);
//			usleep(10000);
//		}
//	}
//	return ((void *) 1);
//}
//void servomove(){
//	int i;
//		for (i = 30; i < 110; i += 17) {
//			servo(i,9);
//			usleep(800000);
//			printf("servo begin\n");
//		}
//		for (i = 100; i > 30; i -= 17) {
//			servo(i,9);
//			usleep(800000);
//}
//		servo(75,9);
//}
void *sock(){
	int num = 15,usetime;
	socklen_t sin_size;
	int sockfd,cliefd, recvBytes, res;
	struct timeval tv1, tv2;
	struct timeval timeout,tv;
	fd_set fdr, fdw, fde;
	char ip[] = "119.29.223.68";
	unsigned char forwad[5]={0xff,0x00,0x01,0x00,0xff};
	/*该结构记录主机的信息，包括主机名、地址类型、地址长度等。*/
	struct hostent *host;

	/*定义结构体用来保存套接字信息*/
	struct sockaddr_in servAddr,clieAddr;

	/*这个函数的传入值是域名或者主机名。传出值，是一个hostent的结构。如果函数调用失败，将返回NULL。*/
	if ((host = gethostbyname(ip)) == NULL) {
		perror("fail to get host by name");
		exit(1);
	}
	printf("Success to get host by name...\n");

	/*建立一个socket连接，AF_INET采用IPv4协议，SOCK_STREAM采用流式socket即TCP*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("fail to establish a socket");
		exit(1);
	}
	printf("Success to establish a socket...\n");

	/*初始化结构体*/
	servAddr.sin_family = AF_INET;
	/*套接字使用的端口，htons是将整型变量从主机字节序转变成网络字节序，存储方式变为：高位字节存放在内存的低地址处*/
	servAddr.sin_port = htons(SERVPORT);
	/*表示的是主机的ip地址，注意，这个是以网络字节序存储的。*/
	servAddr.sin_addr = *((struct in_addr *) host->h_addr);
	/*未使用的字段，填充为0*/
	bzero(&(servAddr.sin_zero), 8);

//	if(bind(sockfd,(struct sockaddr *)&servAddr,sizeof(struct sockaddr))==-1)
//		perror("bind");
//
//	if(listen(sockfd,BACKLOG)==-1)
//		perror("listen");
//	sin_size=sizeof(clieAddr);
//	while((cliefd=accept(sockfd,(struct sockaddr*)&clieAddr,&sin_size))==-1)
//		printf("not accept\n");
//	printf("accept success\n");

	TIMEOUT = 10;
	while (num--) {
		/*connect()用来将参数sockfd 的socket 连至参数serv_Addr 指定的网络地址. */
		if (connect(sockfd, (struct sockaddr *) &servAddr,
				sizeof(struct sockaddr_in)) == -1) {
			printf("Restart connect\n");
			sleep(1);
		} else
			num = 0;
	}

	/*设置套接口非阻塞*/
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
		perror("Set sockfd fail...\n");
		close(sockfd);
	}
	while (1) {
		/*将select()监视的读、写、异常的文件描述符集合清除*/
		FD_ZERO(&fdr);
		FD_ZERO(&fdw);
		FD_ZERO(&fde);

		/*将标准输入文件描述符和新建描述符添加到select()监视的文件描述符集合中*/
		FD_SET(sockfd, &fdr);
		FD_SET(sockfd, &fdw);
		FD_SET(sockfd, &fde);

		/*select()在被监视窗口等待的时间*/
		timeout.tv_sec = TIMEOUT;
		timeout.tv_usec = 0;

		res = select(sockfd + 1, &fdr, 0, 0, &timeout);
		if (res < 0) {
			perror("select error:\n");
			close(sockfd);
			break;
		}
		if (res == 0) {
			continue;
		}
		if (FD_ISSET(sockfd, &fdr)) {

			recvBytes = recv(sockfd, recvBuf, sizeof(recvBuf), 0);
			if (recvBuf[0] == 0)
			{
				move_flag=0;
//				printf("no data\n");
//				sleep(1);
//				control();
			}
			else {
				Get_uartdata(Rdata,recvBuf);
//				printf("Client1:%x\n", recvBuf[0]);
//				printf("Client2:%x\n", recvBuf[1]);
//				printf("Client3:%x\n", recvBuf[2]);
//				printf("Client4:%x\n", recvBuf[3]);
//				printf("Client5:%x\n", recvBuf[4]);
			}
			memset(recvBuf, 0x00, MAX_DATA);
			usleep(10000);
		}
	}
	return ((void *) 1);

}
