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
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

TaskHandle_t LedTaskHandle;
TaskHandle_t PeriodicTaskHandle;
TaskHandle_t PrinterTaskHandle;

TaskStatus_t TskStatus;
QueueHandle_t MessageQueue;
QueueHandle_t DataQueue;

enum {
	PRINT_CHAR,
	PRINT_UINT8,
	PRINT_UINT16,
	PRINT_UINT32,
};

void vPrintMessage(void  *pvParameters,int t)
{
	vTaskSuspendAll();
	switch(t) {
	case PRINT_CHAR:
	{
		char *s = (char *)pvParameters;
		rprintf("%s",s);
	}
		break;
	case PRINT_UINT8:
	case PRINT_UINT16:
	case PRINT_UINT32:
	{
		u32 d = *(u32 *)pvParameters;
		rprintf("%d\r\n",d);
	}
	default:
		break;
	}
	if(!xTaskResumeAll()) {
		taskYIELD();
	}
}

void vPrinterTask(void *pvParameters)
{
	static char RxBuffer[200];
	static int RxBuffer2[10];
	BaseType_t r;

	for(;;) {
		r = xQueueReceive(MessageQueue,RxBuffer,portMAX_DELAY);
		if(r == pdPASS) {
			vPrintMessage(RxBuffer,PRINT_CHAR);
		}
		r = xQueueReceive(DataQueue,RxBuffer2,portMAX_DELAY);
		if(r == pdPASS) {
			vPrintMessage(RxBuffer2,PRINT_UINT32);
		}
	}
}

void vLedTask(void *pvParameters)
{
	unsigned int led_id[] = {LED1,LED2,LED3};
	unsigned int i = 0;
	TickType_t delay = 500 / portTICK_PERIOD_MS;

	for(;;) {
		led_light_on(led_id[i]);
		vTaskDelay(delay);
		led_light_off(led_id[i]);
		vTaskDelay(delay);

		i++;
		i = i % 3;
	}
}

void vPeriodicTask(void *pvParameters)
{
	TickType_t tick;
	TickType_t period = (*(int *)pvParameters)/portTICK_PERIOD_MS;
	BaseType_t r;

	tick = xTaskGetTickCount();
	for(;;) {
		r = xQueueSend(MessageQueue,(void *)"Tick:",(TickType_t )10);
		if(r != pdPASS) {
			rprintf("\r\nqueue send failed,1\r\n");
		}
		r = xQueueSend(DataQueue,(void *)&tick,(TickType_t )10);
		if(r != pdPASS) {
			rprintf("\r\nqueue send failed,2\r\n");
		}
		vTaskDelayUntil(&tick,period);
	}
}

#if  0
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
	BaseType_t r;
	volatile static int Period = 1000;

	rprintf("sysclk: %d, apb1clk: %d, apb2clk: %d\r\n",
		clktree.sysclk,clktree.apb1clk,clktree.apb2clk);

	// create queue
	MessageQueue = xQueueCreate(10,5);
	if(MessageQueue == 0) {
		rprintf("%s, create MessageQueue failed\r\n",__func__);
		goto end;
	}
	DataQueue = xQueueCreate(1,4);
	if(DataQueue == 0) {
		rprintf("%s, create DataQueue failed\r\n",__func__);
		goto end;
	}

	// create task dynamically
	r = xTaskCreate(vLedTask,"LedTask",256,NULL,2,&LedTaskHandle);
	if(r != pdPASS) {
		rprintf("%s, create LedTask failed, %d\r\n",__func__,r);
		goto end;
	}
	r = xTaskCreate(vPeriodicTask,"PeriodicTask",256,(void *)&Period,1,
			&PeriodicTaskHandle);
	if(r != pdPASS) {
		rprintf("%s, create PeriodicTask failed, %d\r\n",__func__,r);
		goto end;
	}
	r = xTaskCreate(vPrinterTask, "PrinterTask",256,NULL,4,&PrinterTaskHandle);
	if(r != pdPASS) {
		rprintf("%s, create PrinterTask failed, %d\r\n",__func__,r);
		goto end;
	}
	// call scheduler
	vTaskStartScheduler();
end:
	for(;;);
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
