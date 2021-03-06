
#include "../common.h"
#include "interrupt_regs.h"
#include "interrupt.h"
#include "printk.h"
#include "task.h"
#include "../driver/timer.h"
#include "../driver/usr_led.h"


#define IRQ_NUM 128
#define IRQ_NUM_MASK 127


/** 128 irq entry */
static void (*irq_handler_array[IRQ_NUM])(void);



/** Enable */
void int_unmask(int32_t irq_id)
{             
	int32_t interrupt_group = irq_id >> 5 ;
	*(INTC_BASE_PTR + INTC_MIR_CLEAR_BASE + 0x20 * interrupt_group) = 0x01 << (irq_id & 0x1F) ;

}



/** Disable */
void int_mask(int32_t irq_id)
{             
	int32_t interrupt_group = irq_id >> 5 ;
	*(INTC_BASE_PTR + INTC_MIR_SET_BASE + 0x20 * interrupt_group) = 0x01 << (irq_id & 0x1F) ;

}



uint32_t get_act_irqnum(void)
{
	return ( *(INTC_BASE_PTR + INTC_SIR_IRQ) &  IRQ_NUM_MASK);
}



void set_irq_route_prio(uint32_t irq_id ,uint32_t priority ,uint32_t route)
{
	*(INTC_ILR_n_BASE_PTR + irq_id * 0x04) = ( (priority << 0x02) & 0xfc ) | route ;
}



void set_irq_route(uint32_t irq_id ,uint32_t route)
{
	*(INTC_ILR_n_BASE_PTR + irq_id * 0x04) |= route ;
}



void cpsr_enable_irq(void)
{
	asm volatile("mrs r0, CPSR\n\t"
				 "bic r0, r0, #0x80\n\t"
				 "msr CPSR_c, r0");
}



void cpsr_disable_irq(void)
{
	asm volatile("mrs r0, CPSR\n\t"
				 "orr r0, r0, #0x80\n\t"
				 "msr CPSR_c, r0");
}



void cpsr_disable_fiq(void)
{
	/* Disable FIQ in CPSR */
	asm volatile("mrs r0, CPSR\n\t"
				 "orr r0, r0, #0x40\n\t"
				 "msr CPSR_c, r0");
}



void cpsr_enable_fiq(void)
{
	/* Enable FIQ in CPSR */
	asm volatile("mrs r0, CPSR\n\t"
		"bic r0, r0, #0x40\n\t"
		"msr CPSR_c, r0");
}



void set_new_irq_agr(){
	*(INTC_BASE_PTR + INTC_CONTROL) |= NEW_IRQ_AGREE ;
}



uint32_t get_ivt_base(void)
{
	uint32_t intvector_addr;

	asm volatile("mrc p15, 0, %0, c1, c0, 0\n" : "=r" (intvector_addr) : : );
	if (intvector_addr & (1 << 13)) {
		return 0xFFFF0000;
	}
		
	asm volatile("mrc p15, 0, %0, c12, c0, 0\n" : "=r" (intvector_addr) : : );
	return intvector_addr;
}



void irq_init(void)
{
	irq_handler_array_init();
	data_sync_barrier(); 
}



void disable_sys_irq(void)
{
	data_sync_barrier(); 
	cpsr_disable_irq() ;
}



void enable_irq_id(uint8_t irq_id)
{
	switch (irq_id >> 5) {
		case 0:
			*(INTC_BASE_PTR + INTC_MIR_CLEAR0) = 1 << (irq_id & ((1 << 5) - 1)); 
			break;
		case 1:
			*(INTC_BASE_PTR + INTC_MIR_CLEAR1) = 1 << (irq_id & ((1 << 5) - 1)); 
			break;
		case 2:
			*(INTC_BASE_PTR + INTC_MIR_CLEAR2) = 1 << (irq_id & ((1 << 5) - 1));
			break;
		case 3:
			*(INTC_BASE_PTR + INTC_MIR_CLEAR3) = 1 << (irq_id & ((1 << 5) - 1)); 
			break;
	}
}



void disable_irq_id(uint8_t irq_id)
{
	switch (irq_id >> 5) {
		case 0:
			*(INTC_BASE_PTR + INTC_MIR_SET0) = 1 << (irq_id & ((1 << 5) - 1)); 
			break;
		case 1:
			*(INTC_BASE_PTR + INTC_MIR_SET1) = 1 << (irq_id & ((1 << 5) - 1)); 
			break;
		case 2:
			*(INTC_BASE_PTR + INTC_MIR_SET2) = 1 << (irq_id & ((1 << 5) - 1)); 
			break;
		case 3:
			*(INTC_BASE_PTR + INTC_MIR_SET3) = 1 << (irq_id & ((1 << 5) - 1)); 
			break;
	}
}



void irq_handler_array_init(void)
{
	int i;

	for (i = 0; i < IRQ_NUM; i++) {
		irq_handler_array[i] = NULL;
	}
}



void irq_isr_bind(uint8_t irq_id, void (*handler)(void))
{
	irq_handler_array[irq_id] = handler;
	*(INTC_ILR_n_BASE_PTR + irq_id) = (0 << 2) | (0 << 0) ;
}



void irq_isr_unbind(uint8_t irq_id)
{
	*(INTC_ILR_n_BASE_PTR + irq_id) = 0 ;
	irq_handler_array[irq_id] = NULL;
}

/****************************************************************************************/

void __attribute__((interrupt("IRQ"))) irqs_handler(void)	
{
	/** 獲得 irq number以判斷是觸發那種中斷 */
	uint8_t irq_id = get_act_irqnum();

	/** 根據獲得的irq num, 執行陣列中對應的函式 */
	if (irq_handler_array[irq_id]) {
		(*irq_handler_array[irq_id])();
		
	} else {
		return ;
	}

	data_sync_barrier();
	*(INTC_BASE_PTR + INTC_CONTROL) = (NEW_IRQ_AGREE << 0); 
}

/****************************************************************************************/

void timer0_isr(uint32_t *usrTaskContextOld)
{
	(DMTIMER0_BASE_PTR_t->IRQSTATUS) = (1 << 1);
	toggle_usrled(3);


	/** Save old context */
	curr_running_task->task_context = (struct TASK_CONTEXT *)usrTaskContextOld ;


	/** Change the task status to ready */
	curr_running_task->task_status = TASK_READY ;
	curr_running_task = NULL ;


	/** prepare run_first_sched() context */
	set_sched_context() ;


	/** 讓下一個irq能觸發 */
	*(INTC_BASE_PTR + INTC_CONTROL) = (NEW_IRQ_AGREE << 0); 


	call_sched((uint32_t)schedFuncContextSPtr) ;
}



/* 2021/1/15--Not work */
void timer2_isr(void)
{	
	toggle_usrled(2);
	toggle_usrled(1);
	toggle_usrled(0);
	(DMTIMER2_BASE_PTR_t->IRQSTATUS) = (1 << 1);
}

/****************************************************************************************/

#define EXCEPT_NUMS		7 	/* Not include reset */

void set_exception_entry(uint32_t *exept_vec_base)
{
	uint32_t *base = exept_vec_base ;
	base += 1 ;
	for (int i=0 ; i<EXCEPT_NUMS; i++) {
		*base = 0xe59ff014 ;
		base += 1 ;
	}
}

/****************************************************************************************/

void __attribute__((interrupt("PABT"))) prefetch_abort_handler(uint32_t r0)
{
	printk("In prefetch_abort_handler\r\n");

	printk("task_info addr :%p\r\n" ,curr_running_task) ;
	printk("pgt base addr :%p\r\n" ,curr_running_task->pgtbase) ;
	printk("lr_abort :%x\r\n\r\n" ,r0) ;

	struct TASK_CONTEXT *ctx = curr_running_task->task_context ;
	printk("task_context sp :%p\r\n" ,ctx->r0) ;
	printk("r0 :%x   r1 :%x   r2 :%x   r3 :%x\r\n" ,ctx->r0 ,ctx->r1 ,ctx->r2 ,ctx->r3) ;
	printk("r4 :%x   r5 :%x   r7 :%x   r7 :%x\r\n" ,ctx->r4 ,ctx->r5 ,ctx->r6 ,ctx->r7) ;
	printk("r8 :%x   r9 :%x   r10 :%x   r11 :%x\r\n" ,ctx->r8 ,ctx->r9_return_lr ,ctx->r10 ,ctx->r11) ;
	printk("r12 :%x   r14 :%x\r\n" ,ctx->r12_spsr ,ctx->lr) ;

	set_wdt_count(WATCHDOG_BASE, 0xfffffff0) ;
	enable_watchdog(WATCHDOG_BASE) ;
	for(;;) ;
}



void __attribute__((interrupt("DABT"))) data_abort_handler(uint32_t r0)
{
	printk("In data_abort_handler\r\n\r\n");

	printk("task_info addr :%p\r\n" ,curr_running_task) ;
	printk("pgt base addr :%p\r\n" ,curr_running_task->pgtbase) ;
	printk("lr_abort :%x\r\n\r\n" ,r0) ;

	struct TASK_CONTEXT *ctx = curr_running_task->task_context ;
	printk("task_context sp :%p\r\n" ,ctx->r0) ;
	printk("r0 :%x   r1 :%x   r2 :%x   r3 :%x\r\n" ,ctx->r0 ,ctx->r1 ,ctx->r2 ,ctx->r3) ;
	printk("r4 :%x   r5 :%x   r7 :%x   r7 :%x\r\n" ,ctx->r4 ,ctx->r5 ,ctx->r6 ,ctx->r7) ;
	printk("r8 :%x   r9 :%x   r10 :%x   r11 :%x\r\n" ,ctx->r8 ,ctx->r9_return_lr ,ctx->r10 ,ctx->r11) ;
	printk("r12 :%x   r14 :%x\r\n" ,ctx->r12_spsr ,ctx->lr) ;

	set_wdt_count(WATCHDOG_BASE, 0xfffffff0) ;
	enable_watchdog(WATCHDOG_BASE) ;
	for(;;) ;
}