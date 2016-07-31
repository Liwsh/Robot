/*
 * socket.c
 *
 *  Created on: 2016��7��10��
 *      Author: Liwensheng
 *********************************************************************************************************
 ** �������� ��sock(int argc, char *argv[])
 ** �������� �����ݽ���
 ** ��ڲ��� ����
 ** ���ڲ��� ����
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
	/*�ýṹ��¼��������Ϣ����������������ַ���͡���ַ���ȵȡ�*/
	struct hostent *host;

	/*����ṹ�����������׽�����Ϣ*/
	struct sockaddr_in servAddr;

	/*��������Ĵ���ֵ����������������������ֵ����һ��hostent�Ľṹ�������������ʧ�ܣ�������NULL��*/
	if ((host = gethostbyname(ip)) == NULL) {
		perror("fail to get host by name");
		exit(1);
	}
	printf("Success to get host by name...\n");

	/*����һ��socket���ӣ�AF_INET����IPv4Э�飬SOCK_STREAM������ʽsocket��TCP*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("fail to establish a socket");
		exit(1);
	}
	printf("Success to establish a socket...\n");

	/*��ʼ���ṹ��*/
	servAddr.sin_family = AF_INET;
	/*�׽���ʹ�õĶ˿ڣ�htons�ǽ����ͱ����������ֽ���ת��������ֽ��򣬴洢��ʽ��Ϊ����λ�ֽڴ�����ڴ�ĵ͵�ַ��*/
	servAddr.sin_port = htons(SERVPORT);
	/*��ʾ����������ip��ַ��ע�⣬������������ֽ���洢�ġ�*/
	servAddr.sin_addr = *((struct in_addr *) host->h_addr);
	/*δʹ�õ��ֶΣ����Ϊ0*/
	bzero(&(servAddr.sin_zero), 8);

	/*connect()����������sockfd ��socket ��������serv_Addr ָ���������ַ. */
	if (connect(sockfd, (struct sockaddr *) &servAddr,
			sizeof(struct sockaddr_in)) == -1) {
		perror("fail to connect the socket");
		exit(1);
	}
	printf("Success to connect the socket...\n");
	/*��һ��ʮ���������ֽ���ת��Ϊ���ʮ����IP��ʽ���ַ�����*/
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

		/*�����ܵ�������Ϣ���ݸ���ȡ�����*/
		pthread_mutex_lock(&mutex1);
		angch_flag = Get_uartdata(recvBytes, recvBuf);
		pthread_mutex_unlock(&mutex1);
		/*����������������*/
		memset(recvBuf, 0x00, MAX_DATA);
	}
	close(sockfd);
}

