#include <common.h>
#include <key.h>
#include <speaker.h>
#include <key_task.h>

TaskHandle_t KeyGetHandle = NULL;
TaskHandle_t KeyPrcHandle = NULL;
TaskHandle_t KeyWthHandle = NULL;
QueueHandle_t KeyPrcQueue = NULL;
SemaphoreHandle_t KeyPresSema = NULL;

void vKeyWatcherTask(void *pvParameters)
{
	BaseType_t r;

	for(;;) {
		r = xSemaphoreTake(KeyPresSema,portMAX_DELAY);

		vTaskSuspendAll();
		if(r == pdTRUE) {
			rprintf("%s, key sema taken\r\n",__func__);
		} else {
			rprintf("%s, key sema taken failed\r\n",__func__);
		}
		xTaskResumeAll();
	}
}

void vKeyGetTask(void *pvParameters)
{
	TickType_t period = 2,tick = 0,cnt = 0,wait = 10;
	u32 k;

	for(;;) {
		tick = xTaskGetTickCount();
		vTaskDelayUntil(&tick,period);

		k = read_key();
		if(k == 0) {
			cnt = 0;
			continue;
		}
		cnt++;
		if(cnt < 6)
			continue;
		cnt = 0;

		vTaskSuspendAll();
		rprintf("%s, %x\r\n",__func__,k);
		xTaskResumeAll();

		xQueueSend(KeyPrcQueue,(void *)&k,wait);
	}
}

static void vKeyPrcFunction(u32 key)
{
	BaseType_t r;

	vTaskSuspendAll();
	rprintf("%s, process key %x\r\n",__func__,key);
	xTaskResumeAll();
	speaker_beep(100,100);
	r = xSemaphoreGive(KeyPresSema);
	if(r != pdTRUE) {
		vMsgPrint("vKeyPrcFunction, give a sema failed\r\n",'s');
	}
}

void vKeyPrcTask(void *pvParameters)
{
	u32 k;
	BaseType_t r;

	for(;;) {
		r = xQueueReceive(KeyPrcQueue,(void *)&k,portMAX_DELAY);
		if(r != pdPASS)
			continue;
		switch(k) {
		case KEY2_ID:
			// TODO: add process code here
			vKeyPrcFunction(k);
			break;
		case KEY3_ID:
			// TODO: add process code here
			vKeyPrcFunction(k);
			break;
		case KEY4_ID:
			// TODO: add process code here
			vKeyPrcFunction(k);
			break;
		default:
			break;
		}
	}
}

int xKeyTaskConstructor(void)
{
	BaseType_t r;
	int depth = 10,width = 4,stk = 256;

	rprintf("%s, depth = %d, width = %d, stk = %d\r\n",__func__,depth,width,stk);
	// create queue
	// queue width is the number of bytes, key value is a 32 bits number,
	// so the width is 4.
	KeyPrcQueue = xQueueCreate(depth,width);
	if(KeyPrcQueue == 0) {
		rprintf("%s, create KeyPrcQueue failed\r\n",__func__);
		return -1;
	}
	KeyPresSema = xSemaphoreCreateBinary();
	if(KeyPresSema == NULL) {
		rprintf("%s, create KeyPresSema failed\r\n",__func__);
		return -1;
	}
	// create task
	r = xTaskCreate(vKeyGetTask,"vKeyGetTask",stk,NULL,2,&KeyGetHandle);
	if(r != pdPASS) {
		rprintf("%s, create vKeyGetTask failed\r\n",__func__);
		return -1;
	}
	r = xTaskCreate(vKeyPrcTask,"vKeyPrcTask",stk,NULL,2,&KeyPrcHandle);
	if(r != pdPASS) {
		rprintf("%s, create vKeyPrcTask failed\r\n",__func__);
		return -1;
	}
	r = xTaskCreate(vKeyWatcherTask,"vKeyWatcher",stk,NULL,2,&KeyWthHandle);
	if(r != pdPASS) {
		rprintf("%s, create vKeyWatcherTask failed\r\n",__func__);
		return -1;
	}
	return 0;
}
