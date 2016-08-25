#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "udpcap.h"
int udpcap() {

	/*声明并初始化一个客户端的socket地址结构*/
	struct sockaddr_in serv_addr;
	struct hostent *host;

	char ip[] = "119.29.223.68";

	if ((host = gethostbyname(ip)) == NULL) {
		perror("fail to get host by name");
		return 0;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Create Socket Failed:");
		return 0;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr = *((struct in_addr *) host->h_addr);
	serv_addr.sin_port = htons(UDPPORT);
	bzero(&(serv_addr.sin_zero), 8);

	/* 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，
	* 用于后面的连接向服务器发起连接，连接成功后sockfd代表了客户端和服务器的一个socket连接
	*/

	if (connect(sockfd, (struct sockaddr*) &serv_addr,
			sizeof(struct sockaddr_in)) == -1) {
		perror("Can Not Connect To Server IP\n");
		return 0;
	}
	// 输入文件名 并放到缓冲区buffer中等待发送
	char file_name[FILE_NAME_MAX_SIZE + 1]="face.jpg";
//	bzero(file_name, FILE_NAME_MAX_SIZE + 1);
	printf("Please Input File Name On Server:\t");
	printf("%s\n",file_name);
	sleep(3);
//	scanf("%s", file_name);
	char buffer[BUFFER_SIZE];
//	bzero(buffer, BUFFER_SIZE);
//	strncpy(buffer, file_name,
//			strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

	// 向服务器发送buffer中的数据
//	if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0) {
//		perror("Send File Name Failed:");
//		exit(1);
//	}
	// 打开文件，准备写入
	FILE *fp = fopen(file_name, "r");
	if (NULL == fp) {
		printf("File:%s Not Found\n", file_name);
	} else {
		bzero(buffer, BUFFER_SIZE);
		int length = 0;        // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止

		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
			if (send(sockfd, buffer, length, 0) < 0) {
				printf("Send File:%s Failed./n", file_name);
				break;
			}
			bzero(buffer, BUFFER_SIZE);
		}
	}       // 接收成功后，关闭文件，关闭socket
	printf("Receive File:\t%s From Server IP Successful!\n", file_name);
	close(fp);
	close(sockfd);
	return 0;
}

