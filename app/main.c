#include <string.h>
#include <common.h>
#include <stm32f10x_system.h>
#include <led.h>
#include <key.h>
#include <motor.h>
#include <infrared.h>

//#define CONFIG_MOTOR_TEST
#define CONFIG_INFRARED_TEST

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
	key_init(KEY2_ID | KEY3_ID | KEY4_ID);
#ifdef CONFIG_MOTOR_TEST
	motor_init();
#endif
#ifdef CONFIG_INFRARED_TEST
	infrared_init();
#endif
	// enable gloabl interrupts
	__enable_irq();
	rprintf("%s done! \r\n",__func__);
}

#ifndef CONFIG_USE_FREERTOS
void serial_loopback(void);
void leds_on(void);

int main(int argc, const char *argv[])
{
	u32 cpuid = system_get_cpuid();

	rprintf("cpuid: %x\r\n",cpuid);
	rprintf("sysclk: %d, apb1clk: %d, apb2clk: %d\r\n",
		clktree.sysclk,clktree.apb1clk,clktree.apb2clk);

	system_systick_run(1);

	//serial_loopback();

	leds_on();
	return 0;
}
#else
#include <led_task.h>
#include <key_task.h>

void vTaskInfoInit(void);

int main(int argc, const char *argv[])
{
	int r;

	rprintf("SYSCLK: %d, APB1CLK: %d, APB2CLk: %d\r\n",
		clktree.sysclk,clktree.apb1clk,clktree.apb2clk);
#ifdef CONFIG_MOTOR_TEST
	motor_test();
#endif
#ifdef CONFIG_INFRARED_TEST
	infrared_test();
#endif
	// led task construction
	r = xLedTaskConstructor();
	if(r) {
		rprintf("%s, xLedTaskConstructor failed\n",__func__);
		return -1;
	}
	r =xKeyTaskConstructor();
	if(r) {
		rprintf("%s, xKeyTaskConstructor failed\n",__func__);
		return -1;
	}

	vTaskInfoInit();

	// call scheduler
	vTaskStartScheduler();

	for(;;);
	return 0;
}
#endif
