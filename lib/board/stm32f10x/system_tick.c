#include <stm32f10x_system.h>

int systick_config(u32 ticks)
{
	if(ticks > SysTick_LOAD_RELOAD_Msk)
		return -1;
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->LOAD = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;
	// set SysTick priority = 15
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL  = 0;
	// enable IRQ, use processor clock
	SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk);

	return 0;
}

void systick_run(int yes)
{
	if(yes)
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	else
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
