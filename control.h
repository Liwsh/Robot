/*
 * control.h
 *
 *  Created on: 2016��8��6��
 *      Author: Administrator
 */

#ifndef CONTROL_H_
#define CONTROL_H_

typedef struct Navi{
    int dir;	//����
    int angle;	//�Ƕ�
    int tangle; //ת���Ƕ�
}navi;

typedef struct Node{
    navi data;
    struct Node *prior;         //ָ��ǰ�����
    struct Node *current;		//ָ��ǰ���
    struct Node *next;          //ָ���̽��
}Node;

int lflag,rflag;
int pflag,nflag;
int preangle,nowangle;
int cangle;
int mov;
int calangle();
int judge();
void control();

#endif /* CONTROL_H_ */
