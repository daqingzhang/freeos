#include <common.h>
#include <stm32f10x_system.h>

static void handler_info(int id)
{
	while(1);
}

void NMI_Handler(void)
{
	handler_info(0);
}

void HardFault_Handler(void)
{
	handler_info(0);
}

void MemManage_Handler(void)
{
	handler_info(0);
}

void BusFault_Handler(void)
{
	handler_info(0);
}

void UsageFault_Handler(void)
{
	handler_info(0);
}

void DebugMon_Handler(void)
{
	handler_info(0);
}

#ifndef CONFIG_USE_FREERTOS
void SVC_Handler(void)
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
#endif
