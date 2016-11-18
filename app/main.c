#include <string.h>
#include <common.h>
#include <stm32f10x_system.h>

const char *logo = "0x12345678abcd";
int a = 1;
int b = 2;
int c = 3;
int array[10];

void serial_loopback(void);

int main(int argc, const char *argv[])
{
	c = 1 << a;
	
	// run OS
	serial_loopback();
	return c;
}

void board_init(void)
{
	// disable global interrupts
	__disable_irq();

	// init system clock
	system_init_clock();

	// set priority group
	system_set_priority_group(NVIC_PRIORITY_GRP4);

	// set vector address
	system_set_vectaddr(NVIC_VECTOR_BASE_FLASH,0);

	// config system tick
	system_systick_config(0x00FFFFFF);

	// config pinmux & GPIO

	// config serial
	serial_init(USART1_ID);
	serial_init(USART2_ID);

	// config LED

	// config KEY

	// enable gloabl interrupts
	__enable_irq();
}

void serial_loopback(void)
{
	char str[80];

	serial_puts(USART1_ID,"input <quit> to exit\n");

	while(1) {
		serial_puts(USART1_ID,"input a string: \n");
		serial_gets(USART1_ID, str);
		serial_puts(USART1_ID,"string is: \n");
		serial_puts(USART1_ID, str);

		if(!strcmp(str,"quit"))
			break;
	}
	rprintf("%s,exit\n");
}
