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
	rprintf("%s done! \r\n",__func__);
}

#ifndef CONFIG_USE_FREERTOS
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
#endif

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

#ifdef CONFIG_USE_FREERTOS
#include <ledtask.h>

TaskStatus_t TskStatus;

#if 0
void vWatchTask(void *pvParameters)
{
	TaskStatus_t *pTskStatus = &TskStatus;
	TaskHandle_t TskHandle = NULL;

	for(;;) {
		TskHandle = xTaskGetHandle("LedTask");
		if(TskHandle) {
			vTaskGetInfo(TskHandle,
					pTskStatus,
					pdTRUE,
					eInvalid);
		}
	}
}
#endif

int main(int argc, const char *argv[])
{
	int r;

	rprintf("sysclk: %d, apb1clk: %d, apb2clk: %d\r\n",
		clktree.sysclk,clktree.apb1clk,clktree.apb2clk);

	// led task construction
	r = xLedTaskConstructor();
	if(r) {
		rprintf("%s, xLedTaskConstructor failed\n",__func__);
		return -1;
	}
	// call scheduler
	vTaskStartScheduler();

	for(;;);
}

void vMsgPrint(void  *pvParameters,char c)
{
	vTaskSuspendAll();
	switch(c) {
	case 's':
	{
		char *s = (char *)pvParameters;
		rprintf("%s",s);
	}
		break;
	case 'x'://u32
	{
		u32 d = *(u32 *)pvParameters;
		rprintf("%8x",d);
	}
		break;
	case 'h'://u16
	{
		u16 d = *(u16 *)pvParameters;
		rprintf("%4x",d);
	}
		break;
	case 'b'://u8
	{
		u8 d = *(u8 *)pvParameters;
		rprintf("%2x",d);
	}
		break;
	case 'c':
	{
		char d = *(char *)pvParameters;
		rprintf("%c",d);
	}
		break;
	case 'd':
	{
		u32 d = *(char *)pvParameters;
		rprintf("%d",d);
	}
		break;
	default:
		break;
	}
	if(!xTaskResumeAll()) {
		taskYIELD();
	}
}

/*
 * vApplicationTickHook - if configUSE_TICK_HOOK is set 1, this
 * subroutine will be called.
 */
void vApplicationTickHook( void )
{

}

/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
 * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
 * task.  It is essential that code added to this hook function never attempts
 * to block in any way (for example, call xQueueReceive() with a block time
 * specified, or call vTaskDelay()).  If the application makes use of the
 * vTaskDelete() API function (as this demo application does) then it is also
 * important that vApplicationIdleHook() is permitted to return to its calling
 * function, because it is the responsibility of the idle task to clean up
 * memory allocated by the kernel to any task that has since been deleted.
 */
void vApplicationIdleHook( void )
{
    
}

/* vApplicationMallocFailedHook() will only be called if
 * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
 * function that will get called if a call to pvPortMalloc() fails.
 * pvPortMalloc() is called internally by the kernel whenever a task, queue,
 * timer or semaphore is created.  It is also called by various parts of the
 * demo application.  If heap_1.c or heap_2.c are used, then the size of the
 * heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
 * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
 * to query the size of free heap space that remains (although it does not
 * provide information on how the remaining heap might be fragmented).
 */
void vApplicationMallocFailedHook( void )
{
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/*
 * Run time stack overflow checking is performed if
 * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.
 * This hook function is called if a stack overflow is detected.
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	taskDISABLE_INTERRUPTS();
	for( ;; );
}
#endif
