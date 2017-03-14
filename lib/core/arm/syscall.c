#include <common.h>
#include <stm32f10x_system.h>

static void handler_info(int id)
{
	while(1);
}

void NMI_Handler(void)
{
	rprintf("%s\n",__func__);
	handler_info(2);
}

void HardFault_Handler(void)
{
	rprintf("%s\n",__func__);
	handler_info(3);
}

void MemManage_Handler(void)
{
	rprintf("%s\n",__func__);
	handler_info(4);
}

void BusFault_Handler(void)
{
	rprintf("%s\n",__func__);
	handler_info(5);
}

void UsageFault_Handler(void)
{
	rprintf("%s\n",__func__);
	handler_info(6);
}

void DebugMon_Handler(void)
{
	rprintf("%s\n",__func__);
	handler_info(7);
}

#ifndef CONFIG_USE_FREERTOS
void SVC_Handler(void)
{
	rprintf("%s\n",__func__);
}

void PendSV_Handler(void)
{
	rprintf("%s\n",__func__);
}

void SysTick_Handler(void)
{
	static int tick = 0;
	unsigned int reg = SysTick->CTRL;

	system_systick_clr_pending();
	rprintf("%x, %d\n", reg,tick++);
}
#endif
