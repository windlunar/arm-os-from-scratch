
#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

#include "../common.h"
#include "interrupt_regs.h"


//void irqs_handler(uint32_t *usrTaskContextOld);
void __attribute__((interrupt("IRQ"))) irqs_handler(void) ;
void timer0_ISR(uint32_t *usrTaskContextOld) ;
void timer2_ISR(void) ;

/****************************************************************************************/
void SYSTEM_INT_Enable(int32_t interruptID) ;
void SYSTEM_INT_disable(int32_t interruptID) ;
uint32_t getActivateIrqNum(void) ;

void 
setIntRouteAndPriority(uint32_t interruptID ,uint32_t priority ,uint32_t route);

void 
setIntRoute(uint32_t interruptID ,uint32_t route);

void cpsrEnableIRQ(void);
void cpsrDisableIRQ(void);
void cpsrDisableFIQ(void);
void cpsrEnableFIQ(void);
void setNewIrqAgr();


/****************************************************************************************/

uint32_t getIntVectorAddr(void);
void interrupt_init(void);
void disableIrqThroughCpsr(void);
void eableINT_NUM(uint8_t irq_num);
void disableINT_NUM(uint8_t irq_num);
void irq_isr_bind(uint8_t irq_num, void (*handler)(void));
void irq_isr_unbind(uint8_t irq_num);
/****************************************************************************************/
void set_exception_entry(uint32_t *exept_vec_base);

/****************************************************************************************/
void __attribute__((interrupt("ABORT"))) prefetch_abort_handler(void) ;
void __attribute__((interrupt("ABORT"))) data_abort_handler(void) ;

/****************************************************************************************/

#endif