#include <common.h>
#include <key.h>
#include <key_task.h>

TaskHandle_t KeyGetHandle;
TaskHandle_t KeyPrcHandle;
QueueHandle_t KeyPrcQueue;

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
	vTaskSuspendAll();
	rprintf("%s, process key %x\n",__func__,key);
	xTaskResumeAll();
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
	int depth = 10,width = 1,stk = 256;

	rprintf("%s, depth = %d, width = %d, stk = %d\r\n",__func__,depth,width,stk);
	// create queue
	KeyPrcQueue = xQueueCreate(depth,width);
	if(KeyPrcQueue == 0) {
		rprintf("%s, create KeyPrcQueue failed\r\n",__func__);
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
	return 0;
}
