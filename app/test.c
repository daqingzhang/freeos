#include <string.h>
#include <common.h>
#include <stm32f10x_system.h>
#include <led.h>
#include <key.h>

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
		led_flash(LED1,500,500);
		led_flash(LED2,500,500);
		led_flash(LED3,500,500);
	}
}
