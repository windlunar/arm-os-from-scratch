

#include "syscall.h"
#include "../klib/print.h"
#include "debug.h"
#include "task.h"
#include "../klib/queue.h"


void syscall_handler(uint32_t syscall_id ,uint32_t *usrTaskContextOld ,void *args) ;
void syscall_handler(uint32_t syscall_id ,uint32_t *usrTaskContextOld ,void *args){
    switch (syscall_id)
    {
    case SYSCALL_ID_print_hello:
        __print_hello(*(uint32_t *)args) ;
        break;

    case SYSCALL_ID_yield:
        __yield(usrTaskContextOld) ;
        break;   

    case SYSCALL_ID_get_tid:
        __get_tid((uint32_t *)args) ;
        break;  

    default:
        break;
    }
}


void  __print_hello(uint32_t input)
{
    kprintf("Hello! This is my first system call,Input value =%d\r\n" ,input) ;
    //readCpsrMode();

}

void __yield(uint32_t *usrTaskContextOld)
{
	// Save old context
	curr_running_task->task_context_sp = (USR_TASK_CONTEXT_t *)usrTaskContextOld ;

	// Change the task status to ready
	curr_running_task->task_status = TASK_READY ;
	curr_running_task = NULL ;

	//prepare sched() context
	schedFuncContextPrepare();

	//dataSyncBarrier();

	_call_sched((uint32_t)schedFuncContextSPtr) ;
}

void __get_tid(uint32_t *tid_return)
{
    //kprintf("In get tid\r\n");
    *tid_return = curr_running_task->task_id ;
}


/************************************************************************************************/
