#include <stm32f10x_system.h>

#define AIRCR_VECTKEY_MASK 0x05FA0000

int nvic_config_channel_priority(u32 id, u32 prio_preempt, u32 prio_sub, int enable)
{
	u32 reg_id = id >> 5;
	u32 mask   = 1 << (id & 0x1f);
	u32 grp,prio;

	if(!enable) {
		NVIC->ICER[reg_id] = mask;
	} else {
		grp = SCB->AIRCR & 0x700;
		if(grp == NVIC_PRIORITY_GRP0) {
			/* preempt = 0 bits, subpriority = 4 bits */
			prio = prio_sub & 0xf;
		} else if(grp == NVIC_PRIORITY_GRP1) {
			/* preempt = 1 bits, subpriority = 3 bits */
			prio = ((prio_preempt & 0x1) << 3) | (prio_sub & 0x7);
		} else if(grp == NVIC_PRIORITY_GRP2) {
			/* preempt = 2 bits, subpriority = 2 bits */
			prio = ((prio_preempt & 0x3) << 2) | (prio_sub & 0x3);
		} else if(grp == NVIC_PRIORITY_GRP3) {
			/* preempt = 3 bits, subpriority = 1 bits */
			prio = ((prio_preempt & 0x7) << 1) | (prio_sub & 0x1);
		} else {
			/* preempt = 4 bits, subpriority = 0 bits */
			prio = prio_preempt & 0xf;
		}
		NVIC->IP[id] = prio;
		NVIC->ICER[reg_id] = mask;
	}
	return 0;
}

int nvic_set_priority_group(u32 grp)
{
	grp &= (0x7 << 8);
	SCB->AIRCR = AIRCR_VECTKEY_MASK | grp;
	return 0;
}

int nvic_set_vector_addr(u32 base, u32 offset)
{
	SCB->VTOR = base | (offset & 0x1FFFFF80);
	return 0;
}

int nvic_set_system_lp(u32 lp_mode, int lp_enable)
{
	if(lp_enable)
		SCB->SCR |= lp_mode;
	else
		SCB->SCR &= ~lp_mode;
	return 0;
}
