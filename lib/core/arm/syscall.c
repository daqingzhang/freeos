#include <common.h>
#include <stm32f10x_system.h>

void NMI_Handler(void)
{

}

void HardFault_Handler(void)
{

}

void MemManage_Handler(void)
{

}

void BusFault_Handler(void)
{

}

void UsageFault_Handler(void)
{

}

void SVC_Handler(void)
{

}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{

}

void SysTick_Handler(void)
{
	static int tick = 0;
	unsigned int reg = SysTick->CTRL;

	system_systick_clr_pending();
	rprintf("%x, %d\r\n", reg,tick++);
}
