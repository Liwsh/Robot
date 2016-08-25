/*
 * control.h
 *
 *  Created on: 2016年8月6日
 *      Author: Administrator
 */

#ifndef CONTROL_H_
#define CONTROL_H_

typedef struct Navi{
    int dir;	//方向
    int angle;	//角度
    int tangle; //转动角度
}navi;

typedef struct Node{
    navi data;
    struct Node *prior;         //指向前驱结点
    struct Node *current;		//指向当前结点
    struct Node *next;          //指向后继结点
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
