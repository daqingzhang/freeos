#ifndef __STM32F10X_SYSTEM_H__
#define __STM32F10X_SYSTEM_H__
#include <stm32f10x.h>

#ifndef assert_param
#define assert_param(...) do{} while(0)
#endif

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

#define NVIC_VECTOR_BASE_FLASH	(0x20000000)
#define NVIC_VECTOR_BASE_SRAM	(0x08000000)

#define NVIC_LP_SEVONPEND	(0x10)
#define NVIC_LP_SLEEPDEEP	(0x04)
#define NVIC_LP_SLEEPONEXIT	(0x02)

#define NVIC_PRIORITY_GRP0	(0x700) /*!< 0 bits for preemption priority
		     	  		4 bits for subpriority */
#define NVIC_PRIORITY_GRP1	(0x600) /*!< 1 bits for preemption priority
		     	  		3 bits for subpriority */
#define NVIC_PRIORITY_GRP2	(0x500) /*!< 2 bits for preemption priority
		     	  		2 bits for subpriority */
#define NVIC_PRIORITY_GRP3	(0x400) /*!< 3 bits for preemption priority
					1 bits for subpriority */
#define NVIC_PRIORITY_GRP4	(0x300) /*!< 4 bits for preemption priority
					0 bits for subpriority */
int nvic_set_priority_group(u32 grp);
int nvic_set_vector_addr(u32 base, u32 offset);
int nvic_set_system_lp(u32 lp_mode, int lp_enable);
int nvic_config_channel_priority(u32 id, u32 prio_preempt, u32 prio_sub, int enable);

int systick_config(u32 ticks);
void systick_run(int yes);

#endif
