/*
 * socket.c
 *
 *  Created on: 2016年7月10日
 *      Author: Liwensheng
 *********************************************************************************************************
 ** 函数名称 ：sock(int argc, char *argv[])
 ** 函数功能 ：数据接收
 ** 入口参数 ：无
 ** 出口参数 ：无
 *********************************************************************************************************
 */
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include "socket.h"
#include "robot.h"
#include "analy.h"

int sock() {
	int sockfd, sendBytes, recvBytes;
	char ip[] = "119.29.223.68";
	/*该结构记录主机的信息，包括主机名、地址类型、地址长度等。*/
	struct hostent *host;

	/*定义结构体用来保存套接字信息*/
	struct sockaddr_in servAddr;

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

	/*connect()用来将参数sockfd 的socket 连至参数serv_Addr 指定的网络地址. */
	if (connect(sockfd, (struct sockaddr *) &servAddr,
			sizeof(struct sockaddr_in)) == -1) {
		perror("fail to connect the socket");
		exit(1);
	}
	printf("Success to connect the socket...\n");
	/*将一个十进制网络字节序转换为点分十进制IP格式的字符串。*/
	printf("Welcome to join %s!\n", inet_ntoa(servAddr.sin_addr));

	while (1) {
		if ((recvBytes = read(sockfd, recvBuf, sizeof(recvBuf))) == -1) {
			perror("fail to receive datas");
			exit(1);
		}
		printf("Client1:%x\n", recvBuf[0]);
		printf("Client2:%x\n", recvBuf[1]);
		printf("Client3:%x\n", recvBuf[2]);
		printf("Client4:%x\n", recvBuf[3]);
		printf("Client5:%x\n", recvBuf[4]);

		/*将接受的数据信息传递给读取命令函数*/
		pthread_mutex_lock(&mutex1);
		angch_flag = Get_uartdata(recvBytes, recvBuf);
		pthread_mutex_unlock(&mutex1);
		/*接受命令数组清零*/
		memset(recvBuf, 0x00, MAX_DATA);
	}
	close(sockfd);
}

