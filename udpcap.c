#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "udpcap.h"
int udpcap() {

	/*��������ʼ��һ���ͻ��˵�socket��ַ�ṹ*/
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

	/* ����һ���������˵�socket��ַ�ṹ�����÷������Ǳߵ�IP��ַ���˿ڶ�����г�ʼ����
	* ���ں����������������������ӣ����ӳɹ���sockfd�����˿ͻ��˺ͷ�������һ��socket����
	*/

	if (connect(sockfd, (struct sockaddr*) &serv_addr,
			sizeof(struct sockaddr_in)) == -1) {
		perror("Can Not Connect To Server IP\n");
		return 0;
	}
	// �����ļ��� ���ŵ�������buffer�еȴ�����
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

	// �����������buffer�е�����
//	if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0) {
//		perror("Send File Name Failed:");
//		exit(1);
//	}
	// ���ļ���׼��д��
	FILE *fp = fopen(file_name, "r");
	if (NULL == fp) {
		printf("File:%s Not Found\n", file_name);
	} else {
		bzero(buffer, BUFFER_SIZE);
		int length = 0;        // ÿ��ȡһ�����ݣ��㽫�䷢�͸��ͻ��ˣ�ѭ��ֱ���ļ�����Ϊֹ

		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
			if (send(sockfd, buffer, length, 0) < 0) {
				printf("Send File:%s Failed./n", file_name);
				break;
			}
			bzero(buffer, BUFFER_SIZE);
		}
	}       // ���ճɹ��󣬹ر��ļ����ر�socket
	printf("Receive File:\t%s From Server IP Successful!\n", file_name);
	close(fp);
	close(sockfd);
	return 0;
}

