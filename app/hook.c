#include <string.h>
#include <common.h>
#include <stm32f10x_system.h>
#include <led.h>
#include <key.h>
#include <motor.h>
#include <led_task.h>
#include <key_task.h>

#ifdef CONFIG_TASK_INFO
#define TASK_STATUS_NUM 6

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

static void vTaskInfoCollector(void)
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

static void vTaskInfoPrinter(void)
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
#endif /* CONFIG_TASK_INFO */

/*
 * vApplicationTickHook - if configUSE_TICK_HOOK is set 1, this
 * subroutine will be called.
 */
void vApplicationTickHook( void )
{
#ifdef CONFIG_TASK_INFO
	vTaskInfoCollector();
#endif
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
#ifdef CONFIG_TASK_INFO
	vTaskInfoPrinter();
#endif
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
