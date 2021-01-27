

#include "syscall.h"
#include "../klib/print.h"
#include "debug.h"
#include "task.h"
#include "../klib/queue.h"
#include "../klib/mem.h"


void syscall_handler(uint32_t syscall_id ,uint32_t *usrTaskContextOld ,void *args) ;
void syscall_handler(uint32_t syscall_id ,uint32_t *usrTaskContextOld ,void *args)
{
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

    case SYSCALL_ID_exit:
        __exit() ;
        break;  

    case SYSCALL_ID_fork:
        __fork(usrTaskContextOld ,(uint32_t *)args) ;
        break;  

    default:
        break;
    }
}


void  __print_hello(uint32_t input)
{
    kprintf("Hello! This is my first system call,Input value =%d\r\n" ,input) ;
}

void __yield(uint32_t *usrTaskContextOld)
{
	// Save old context
	curr_running_task->task_context = (USR_TASK_CONTEXT_t *)usrTaskContextOld ;

	// Change the task status to ready
	curr_running_task->task_status = TASK_READY ;
	curr_running_task = NULL ;

	//prepare sched() context
	schedFuncContextPrepare();
	_call_sched((uint32_t)schedFuncContextSPtr) ;
}

void __get_tid(uint32_t *tid_return)
{
    *tid_return = curr_running_task->task_id ;
}


// 回收資源
// 將task設定為 terminate
void __exit()
{
    kprintf("task id=%d exit\r\n" ,curr_running_task->task_id) ;
    curr_running_task->task_status = TASK_TERMINATE ;

    //從queue中移除
    remove_from_readylist(curr_running_task) ;

    //釋放空間
    MEM_PART_INFO_t *curr_mpinfo = which_mem_part(curr_running_task->stk_bottom) ;
    free_mem_part(curr_mpinfo) ;
  
    curr_running_task->stk_bottom = NULL ;
    curr_running_task = NULL ;

	//prepare sched() context
	schedFuncContextPrepare();
	_call_sched((uint32_t)schedFuncContextSPtr) ;
}



// 複製一份相同的stack ,跟 TASK_INFO_t 結構體
void __fork(uint32_t *usrTaskContextOld ,uint32_t *args)
{    
    // Alloc 一個 Memory Part,並回傳描述該part的結構體 MEM_PART_INFO_t
    // 並將該 part使用狀況設為 INUSE_FULL ,只屬於這個task使用
    MEM_PART_INFO_t *n_mempart = alloc_one_mem_part();
    n_mempart->part_status = INUSE_FULL ;

    // 分配 描述 task的TASK_INFO_t結構體, 其起始位置設定為 剛剛分配的記憶體空間的起始位址
    TASK_INFO_t *ntask = (TASK_INFO_t *)n_mempart->m_start ;

    // 找目前正在執行的task(父task)的stack空間屬於那一個 memory part
    MEM_PART_INFO_t *curr_mpinfo = which_mem_part(curr_running_task->stk_bottom) ;

    // 複製父task使用的記憶體區段內的data給子task的記憶體區段, 包含stack空間內所有內容
    _memcpy((void *)(n_mempart->m_start)
            ,(void *)(curr_mpinfo->m_start)
            ,PART_SIZE) ;

    // 複製描述 task的TASK_INFO_t結構體
    _memcpy((void *)ntask ,(void *)curr_running_task ,sizeof(TASK_INFO_t));

    // Stack pointer要指向stack中相同的相對位址上
    uint32_t old_sp = (uint32_t)usrTaskContextOld ;
    uint32_t new_sp = (uint32_t)(n_mempart->m_start) ;

    old_sp &= 0xFFF ;
    new_sp |= old_sp ;

    ntask->task_context = (USR_TASK_CONTEXT_t *)new_sp ;

    // 設定子stack的 task id
	taskid++ ;
	ntask->task_id = taskid;

    // 設定TASK_INFO_t結構體的其他內容
    ntask->next_ptr = NULL ;
    ntask->prev_ptr = NULL ;
    ntask->task_status = TASK_READY ;
    ntask->taskCallBack = curr_running_task->taskCallBack ;
    ntask->stk_bottom = n_mempart->m_top - (TASK_STACK_SIZE/4) +1  ;

    
    // 將子task放入 rady list中
    task_enqueue(ntask) ;

    // return child tid
    *args = ntask->task_id ;

    //設定回傳值
    ntask->task_context->r0 = 0 ;
    USR_TASK_CONTEXT_t *old_context = (USR_TASK_CONTEXT_t *)usrTaskContextOld ;
    old_context->r0 = ntask->task_id ;
    
}
/************************************************************************************************/
