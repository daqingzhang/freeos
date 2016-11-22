#ifndef __STM32F10X_SYSTEM_H__
#define __STM32F10X_SYSTEM_H__
#include <stm32f10x.h>

#ifndef assert_param
#define assert_param(...) do{} while(0)
#endif

#define CONFIG_SYSCLK_72MHZ 72000000
#define CONFIG_SYSCLK_64MHZ 64000000
#define CONFIG_APBCLK_72MHZ 72000000
#define CONFIG_APBCLK_36MHZ 36000000
#define CONFIG_APBCLK_32MHZ 32000000

#define CONFIG_SYSCLK  CONFIG_SYSCLK_72MHZ

#define CONFIG_SYSTICK_US(us) (CONFIG_SYSCLK / 1000000 * us)
#define CONFIG_SYSTICK_MS(ms) (CONFIG_SYSCLK / 1000 * ms)

struct clock_tree
{
	u32 sysclk;
	u32 apb1clk;
	u32 apb2clk;
};

extern struct clock_tree clktree;

#define NVIC_VECTOR_BASE_FLASH	(0x08000000)
#define NVIC_VECTOR_BASE_SRAM	(0x20000000)

/*
 * if SCR[4] is set 1, processor will wakeup when a interrupt transmition
 * from a inactive state to pending state. Otherwise will not wakeup.
 */
#define NVIC_LP_SEVONPEND	(0x10)

/*
 * if SCR[2] is set 1, processor will enter deep sleep.
 */
#define NVIC_LP_SLEEPDEEP	(0x04)

/*
 * if SCR[1] is set 1, processor will enter sleep mode on a exit from ISR
 * if SCR[1] is set 0, processor will enter sleep mode when executes WFE or WFI.
 * In the sleep mode, all I/O pins keep the same state as in the running mode.
 * WFI = wait for interrupt
 * WFE = wait for event
 * There are two usage:
 *	sleep-now mode
 *		Enter sleep:
 *			(1) SLEEPDEEP = 0 & SPEEPONEXIT = 0
 *			(2) WFI
 *		Exit sleep
 *			(1) Interrupt occuring
 *	sleep-on-exit mode
 *		Enter sleep:
 *			(1) SLEEPDEEP = 0 & SPEEDONEXIT = 1
 *			(2) WFI
 *		Exit sleep
 *			(1) Interrupt occuring
 */
#define NVIC_LP_SLEEPONEXIT	(0x02)

/*
 * NVIC priority group
 */
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
u32  system_get_cpuid(void);
int  system_init_clock(void);
void system_nmi_set_pending(void);
void system_svc_set_pending(void);
void system_svc_clr_pending(void);
void system_systick_set_pending(void);
void system_systick_clr_pending(void);
int  system_systick_config(u32 ticks);
void system_systick_run(int yes);
void system_softreset(void);
int  system_set_priority_group(u32 grp);
int  system_set_vectaddr(u32 base, u32 offset);
int  system_set_low_power(u32 lp_mode, int lp_enable);
int  system_nvic_set_channel(u32 id, u32 prio_preempt, u32 prio_sub, int enable);

#endif
