#include <stm32f10x_system.h>
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
	nvic_set_priority_group(NVIC_PRIORITY_GRP4);
	nvic_set_vector_addr(NVIC_VECTOR_BASE_FLASH,0);

	// init system tick
	systick_config(0x00FFFFFF);

	// config pinmux & GPIO

	// config serial
	//serial_init();

	// config LED

	// config KEY

	// Enable gloabl interrupts
}
