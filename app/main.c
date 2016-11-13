#include <stm32f10x.h>
#include <misc.h>
#include <common.h>

const char *logo = "0x12345678abcd";
int a = 1;
int b = 2;
int c = 3;
static int d = 5;
int array[10];

int main(int argc, const char *argv[])
{
	c = 1 << a;
	
	// run OS

	return c;
}

void board_init(void)
{
	// disable global interrupts

	// init system clock

	// init NVIC
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
	nvic_priority_group_config(NVIC_PRI_GRP4);
	// init system tick
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	// config pinmux & GPIO

	// config serial
	//serial_init();

	// config LED

	// config KEY

	// Enable gloabl interrupts
}
