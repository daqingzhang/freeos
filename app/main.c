#include <string.h>
#include <common.h>
#include <stm32f10x_system.h>
#include <led.h>

const char *logo = "0x12345678abcd";
int a = 1;
int b = 2;
int c = 3;
int array[10];

void serial_loopback(void);
void leds_on(void);

void board_init(void)
{
	int r = 0;

	// disable global interrupts
	__disable_irq();

	// init system clock
	r = system_init_clock();
	if(r)
		rprintf("system_init_clock failed %d\r\n",r);

	// set priority group
	system_set_priority_group(NVIC_PRIORITY_GRP4);

	// set vector address
	system_set_vectaddr(NVIC_VECTOR_BASE_FLASH,0);

	// config system tick
	r = system_systick_config(CONFIG_SYSTICK_MS(100));
	if(r)
		rprintf("system_systick_config failed %d\r\n",r);

	// config pinmux & GPIO

	// config serial
	serial_init(USART1_ID);
	serial_init(USART2_ID);

	// config TIMER1,TIMER2,TIMER3,TIMER4
	timer_init(TIMER1_ID);
	timer_init(TIMER2_ID);
	timer_init(TIMER3_ID);
	timer_init(TIMER4_ID);

	// config LED
	led_init(LED1|LED2|LED3);

	// config KEY

	// enable gloabl interrupts
	__enable_irq();
}

int main(int argc, const char *argv[])
{
	int r;

	u32 cpuid = system_get_cpuid();

	rprintf("cpuid: %x\r\n",cpuid);
	rprintf("sysclk: %d, apb1clk: %d, apb2clk: %d\r\n",
		clktree.sysclk,clktree.apb1clk,clktree.apb2clk);

	system_systick_run(1);

	//serial_loopback();

	leds_on();
	return c;
}

void serial_loopback(void)
{
	char str[80];

	serial_puts(USART1_ID,"input <quit> to exit\n");

	while(1) {
		serial_puts(USART1_ID,"\ninput a string: ");
		serial_gets(USART1_ID, str);
		serial_puts(USART1_ID,"\nstring is: ");
		serial_puts(USART1_ID, str);

		if(!strcmp(str,"quit"))
			break;
	}
	rprintf("%s,exit\n",__func__);
}

void leds_on(void)
{
	while(1) {
		led_light_flash(LED1,500,500);
		led_light_flash(LED2,500,500);
		led_light_flash(LED3,500,500);
	}
}
