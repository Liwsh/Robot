/*
 * udpcap.h
 *
 *  Created on: 2016Äê8ÔÂ5ÈÕ
 *      Author: Administrator
 */

#ifndef UDPCAP_H_
#define UDPCAP_H_
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>

#define UDPPORT 8080
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

int udpcap();
#endif /* UDPCAP_H_ */
