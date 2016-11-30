#include <string.h>
#include <common.h>
#include <stm32f10x_system.h>
#include <led.h>
#include <key.h>
#include <motor.h>

//#define CONFIG_MOTOR_TEST

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
	// init motor
	motor_init();
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

#else /* CONFIG_USE_FREERTOS defined */

#include <led_task.h>
#include <key_task.h>
#if 0
TaskStatus_t TskStatus;
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

void vTaskInfoInit(void);

int main(int argc, const char *argv[])
{
	int r;

	rprintf("SYSCLK: %d, APB1CLK: %d, APB2CLk: %d\r\n",
		clktree.sysclk,clktree.apb1clk,clktree.apb2clk);

#ifdef CONFIG_MOTOR_TEST
	motor_test();
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

#define TASK_STATUS_NUM 6

struct TaskStateType
{
	const char	*name;
	u32		cnt;
};

struct TaskInfoType
{
	const char *name;
	struct TaskStateType state[TASK_STATUS_NUM];
	TaskHandle_t handle;
};

#define INIT_TASK_STATE(_name,_cnt) 	\
	{				\
		.name = _name,		\
		.cnt = _cnt		\
	}


#define INIT_TASK_INFO(_name)		\
	{				\
		.name = _name,		\
		.handle = NULL,		\
		.state = 		\
			{		\
				INIT_TASK_STATE("running"  ,0),	\
				INIT_TASK_STATE("ready"    ,0),	\
				INIT_TASK_STATE("blocked"  ,0),	\
				INIT_TASK_STATE("suspended",0),	\
				INIT_TASK_STATE("deleted"  ,0),	\
				INIT_TASK_STATE("invalid"  ,0),	\
			},					\
	}

struct TaskInfoType AppTasks[] =
{
	INIT_TASK_INFO("LedDispTask"),
	INIT_TASK_INFO("Led1DlyTask"),
	INIT_TASK_INFO("Led2DlyTask"),
	INIT_TASK_INFO("Led3DlyTask"),
	INIT_TASK_INFO("LedCtrlTask"),
	INIT_TASK_INFO("KeyGetTask" ),
	INIT_TASK_INFO("KeyPrcTask" ),
	INIT_TASK_INFO("KeyWthTask" ),
};

unsigned int uAppTaskTickNum = 0;
unsigned long long uAppTaskTickCnt = 0;

void vTaskInfoInit(void)
{
	struct TaskInfoType *pt = AppTasks;
	int i = 0;

	pt[i++].handle = LedDispHandle;
	pt[i++].handle = Led1DlyHandle;
	pt[i++].handle = Led2DlyHandle;
	pt[i++].handle = Led3DlyHandle;
	pt[i++].handle = LedCtrlHandle;
	pt[i++].handle = KeyGetHandle;
	pt[i++].handle = KeyPrcHandle;
	pt[i++].handle = KeyWthHandle;
/*	rprintf("%s, LedDispHandle = %x, pt[0].handle = %x\r\n",
		__func__,LedDispHandle, pt[0].handle);
*/
}

void vTaskInfoCollector(void)
{
	int i,j,err = 0;
	struct TaskInfoType *pt;
	struct TaskStateType *ps;

	for(i = 0;i < ARRAY_SIZE(AppTasks);i++) {
		pt = AppTasks + i;
		ps = pt->state;

		j = eTaskGetState(pt->handle);
		if(j >= TASK_STATUS_NUM) {
			vTaskSuspendAll();
			rprintf("vTaskInfoCollector, error %d !\r\n",j);
			xTaskResumeAll();
			err = 1;
			break;
		}
		ps[j].cnt++;

		uAppTaskTickNum++;
		uAppTaskTickCnt++;
	}
	if(err) {
		taskDISABLE_INTERRUPTS();
		for( ;; );
	}
}

void vTaskInfoPrinter(void)
{
	int i;
	struct TaskInfoType *pt;
	struct TaskStateType *ps;

	if(uAppTaskTickNum < 1000)
		return;
	uAppTaskTickNum = 0;

	vTaskSuspendAll();
	rprintf("\r\n");
	for(i = 0;i < ARRAY_SIZE(AppTasks);i++) {
		pt = AppTasks + i;
		ps = pt->state;
		rprintf("%s\t, %s = %d\t, %s = %d\t,%s = %d\t,"
			"%s = %d\t, %s = %d\t, %s = %d\r\n",
			pt->name,
			ps[0].name,ps[0].cnt,ps[1].name,ps[1].cnt,
			ps[2].name,ps[2].cnt,ps[3].name,ps[3].cnt,
			ps[4].name,ps[4].cnt,ps[5].name,ps[5].cnt);
		rprintf("\r\n");
	}
	xTaskResumeAll();
}

/*
 * vApplicationTickHook - if configUSE_TICK_HOOK is set 1, this
 * subroutine will be called.
 */
void vApplicationTickHook( void )
{
	vTaskInfoCollector();
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
	vTaskInfoPrinter();
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
#endif /* CONFIG_USE_FREERTOS */
