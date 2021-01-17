/**
 * Queue Implementation
 * 
 * Date : 2020/11/25
 * Author : Yi-Ying Lin
 */ 

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "print.h"
#include "../kernel/task.h"


/** 定義 queue的結構體
 * 包含該queue空間的size(能存放的最大數)
 * 頭尾的index
 * 存放 data的空間(以pointer的形式)
*/
typedef struct{
    int32_t size ; 
    int32_t idxHead ;
    int32_t idxBack ;
    int32_t NumDataInQueue;
    USERTASK_t *qDataTaskStructPtr[TASK_NUM];  //存放 USERTASK_t 結構指標

}QUEUE_TASK_t;

extern QUEUE_TASK_t taskReadyQ ;


/** 各個功能函式宣告, 各函式作用請見 queue.c的註解*/

void queueInit(QUEUE_TASK_t *qPtr ,int qSize);
void queueDelete(QUEUE_TASK_t *qPtr);
int32_t queueIsEmpty(QUEUE_TASK_t *qPtr);
int32_t queueIsFull(QUEUE_TASK_t *qPtr);
int32_t enQueue(QUEUE_TASK_t *qPtr, USERTASK_t *data);
USERTASK_t *deQueue(QUEUE_TASK_t *qPtr);
void printDataInQueue(QUEUE_TASK_t *qPtr);



#endif