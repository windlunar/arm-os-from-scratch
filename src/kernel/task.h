
#ifndef __TASK_H_
#define __TASK_H_

#include "../common.h"
#include "../klib/print.h"
#include "debug.h"

extern int32_t taskid;
/***********************************************************************************************/
#define TASK_RUNNING    0
#define TASK_READY      1
#define TASK_TERMINATE  2
#define TASK_SUSPEND    3
/***********************************************************************************************/
#define TASK_STACK_SIZE 512
#define TASK_NUM_MAX 32

/***********************************************************************************************/

// -----------------
// ......
// lr
// r12
// r11
// r10
// r9
// r8
// r7
// r6
// r5
// r4
// r3
// r2
// r1 -----------------0x9df31000 + 4*1
// r0 -----------------kernel init stack top = 0x9df31000 (set up in start.s)
//                      sched sp (start addr of SCHED_CONTEXT_t schedFuncContextSPtr)
typedef struct
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t lr;

}SCHED_CONTEXT_t;
extern SCHED_CONTEXT_t *schedFuncContextSPtr ; 




// *******************************************************
//
//                 SP------>lr
//                          r12
// push=STMFD SP!           r11
//        |                 r10
//        |                 r9      ^
//        |                 r8      |
//        |                 r7      |
//        |                 r6      |
//        |                 r5      |
//        |                 r4      |
//        V                 r3      |
//                          r2  pop=LDMFD SP!      
//                          r1              
//                          r0 <------SP
//
// *******************************************************
typedef struct
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9_return_lr;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t lr;

}USR_TASK_CONTEXT_t;

struct _TASK
{
    struct _TASK *next_ptr ;
    struct _TASK *prev_ptr ;   

    // task 的 stack空間 的pointer
    uint32_t *stk_bottom ;
    uint32_t *stk_top ;

    // task function pointer
    void (*taskCallBack)() ;

    int32_t task_id;
    int32_t task_status ;

    // task context ,存放在stack中
    USR_TASK_CONTEXT_t *task_context ; 

    //int32_t priority ; //not implement yet ;

};
typedef struct _TASK TASK_INFO_t ;
extern TASK_INFO_t *task_ready_queue_head;
/***********************************************************************************************/

extern TASK_INFO_t task_origin ;
extern uint32_t task_origin_stack[TASK_STACK_SIZE/4] ;
extern TASK_INFO_t *curr_running_task ;
/***********************************************************************************************/
void sched(void);
void schedFuncContextPrepare(void);
extern void _call_sched(uint32_t schedContext) ;    //定義在task_asm.s
void TaskRun(uint32_t *sp);     //輸入參數 stack(Process stack pointer)會存到r0


int32_t taskCreate(TASK_INFO_t *task ,void (*taskFunc)() ,void *stack);
void task_enqueue(TASK_INFO_t *task_node) ;
TASK_INFO_t *task_dequeue() ;
void remove_from_readylist(TASK_INFO_t *task_node);
void print_task_id_from_head() ;

/***********************************************************************************************/
#endif

