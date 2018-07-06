#include "myfifo.h"
#include <stdio.h>
#include <stdlib.h>
//链队列结点结构
typedef struct QNode{
	int data;
	struct QNode *next;
}QNode;
//链队列结构
typedef struct LiQueue{
	QNode *front;
	QNode *rear;
}LiQueue;
//创建链队列
LiQueue initQueue(){
	LiQueue *lq=(LiQueue *)malloc(sizeof(LiQueue));
	if(lq ==NULL){
		return;
	}
	lq->front=lq->rear=NULL;
	return *lq;
}
//判断链队列是否为空
int isEmpty(LiQueue *lq){
	if(lq->front==NULL || lq->rear==NULL){
		return 0;
	}else{
		return 1;
	}
}
//元素进链队列
void enQueue(LiQueue *lq,int x){
	QNode *p;
	p=(QNode *)malloc(sizeof(QNode));
	p->data=x;
	p->next=NULL;
	if(lq->rear==NULL){
		lq->front=lq->rear=p;//如果第一次插入则设置头指针和尾指针为p
	}else{
		lq->rear->next=p;//链队列的尾部插入p
		lq->rear=p;//设置链队列的尾指针指向p
	}
}
//元素出链队列
int deQueue(LiQueue *lq,int *y){
	QNode *p=lq->front;
	if(lq->rear==NULL || lq->front==NULL){
		return 0;
	}
	if(lq->front==lq->rear){
		lq->front=lq->rear=NULL;
	}else{
		lq->front=lq->front->next;
	}
	*y=p->data;
	free(p);
	return 1;
}
//打印链队列
void printQueue(LiQueue lq){
	if(lq.front==NULL || lq.rear==NULL){
		return;
	}
	while(lq.front!=NULL){
		printf("%d\n",lq.front->data);
		lq.front=lq.front->next;
	}
}
void test_q()
{
	int y=0,x=0,z=0;
	LiQueue lq=initQueue();
	enQueue(&lq,1);
	enQueue(&lq,2);
	enQueue(&lq,3);
	enQueue(&lq,4);
	enQueue(&lq,5);
	deQueue(&lq,&x);
	deQueue(&lq,&y);
	deQueue(&lq,&z);
	printQueue(lq);
	printf("出队列元素=%d,%d,%d\n",x,y,z);
}


