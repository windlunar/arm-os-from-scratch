

#include "syscall.h"
#include "../klib/print.h"
#include "debug.h"
#include "task.h"
#include "../klib/queue.h"


void syscall_handler(uint32_t syscall_id ,uint32_t *usrTaskContextOld) ;
void syscall_handler(uint32_t syscall_id ,uint32_t *usrTaskContextOld){
    switch (syscall_id)
    {
    case SYSCALL_ID_print_hello:
        __print_hello() ;
        break;

    case SYSCALL_ID_yield:
        __yield(usrTaskContextOld) ;
        break;   

    default:
        break;
    }
}


void __print_hello(void)
{
    kprintf("Hello! This is my first system call\r\n") ;
    //readCpsrMode();
}

void __yield(uint32_t *usrTaskContextOld){
    // check which task's status is running
    // Then change the task satus back to ready
	for(int32_t id =0 ; id<TASK_NUM; id++)
	{
		if(userTask[id].taskStatus == TASK_RUNNING)
		{
			// Save old context
			userTask[id].usrTaskContextSPtr = (USR_TASK_CONTEXT_t *)usrTaskContextOld ;

			// Change the task status to ready
			userTask[id].taskStatus = TASK_READY ;
			break ;
		}
	}

	//prepare sched() context
	schedFuncContextPrepare();

	//dataSyncBarrier();

	_call_sched((uint32_t)schedFuncContextSPtr) ;
}