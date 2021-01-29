
#ifndef __USYSCALL_H_
#define __USYSCALL_H_

#include "../common.h"
#include "../kernel/syscall.h"
#include "../kernel/task.h"


void __print_hello(uint32_t *input) ;
void __yield(void);
int __gettid(void);
void __exit(void);
int __fork(void);
int __do_taskCreate(void (*taskFunc)() ,int32_t prio);
void *__malloc_blk() ;
void __mfree_blk(void *blk_aval_start) ;
void __get_mblk_list();
int __get_task_priority();

#endif