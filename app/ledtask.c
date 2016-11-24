#include <string.h>
#include <led.h>
#include <ledtask.h>

TaskHandle_t LedDispHandle;
TaskHandle_t Led1DlyHandle;
TaskHandle_t Led2DlyHandle;
TaskHandle_t Led3DlyHandle;
TaskHandle_t LedCtrlHandle;

QueueHandle_t LedDispQueue;
QueueHandle_t LedRspQueue;
QueueHandle_t Led1DlyQueue;
QueueHandle_t Led2DlyQueue;
QueueHandle_t Led3DlyQueue;


static int xLedMsgPack(struct LedMsgFmt *msg,int cmd,int f1,int f2,int f3,int f4)
{
	int i = 0;

	if(msg == NULL)
		return -1;

	msg->head = LED_MSG_HEAD;
	switch(cmd) {
	case LED_CMD_ON:
		msg->cmd  = LED_CMD_ON;
		msg->len  = LED_PLL_ON;
		break;
	case LED_CMD_DLY:
		msg->cmd  = LED_CMD_DLY;
		msg->len  = LED_PLL_DLY;
		break;
	case LED_CMD_RSP:
		msg->cmd  = LED_CMD_RSP;
		msg->len  = LED_PLL_RSP;
		break;
	default:
		return -1;
	}
	msg->pl[i++] = (u8)(LED1);
	msg->pl[i++] = (u8)(f1);
	msg->pl[i++] = (u8)(LED2);
	msg->pl[i++] = (u8)(f2);
	msg->pl[i++] = (u8)(LED3);
	msg->pl[i++] = (u8)(f3);
	memset(&(msg->pl[i]),LED_PL_DUMMY,LED_MSG_PL_SIZE - i);
	msg->crc = CalCRC16(msg->pl,LED_MSG_PL_SIZE);
	return 0;
}

void vLedMsgPrint(struct LedMsgFmt *msg)
{
	int i = 0;

	vTaskSuspendAll();
	rprintf("%2x ",msg->head);
	rprintf("%2x ",msg->cmd);
	rprintf("%4x ",msg->len);
	for(i = 0;i < LED_MSG_PL_SIZE;i++)
		rprintf("%2x ",msg->pl[i]);
	rprintf("%4x\r\n",msg->crc);
	xTaskResumeAll();
}

void vLedDispTask(void *pvParameters)
{
	unsigned int i = 0,led,on;
	BaseType_t r;
	struct LedMsgFmt DispMsg;

	for(;;) {
		r = xQueueReceive(LedDispQueue,(void *)&DispMsg,portMAX_DELAY);
		vLedMsgPrint(&DispMsg);
		if(r != pdPASS)
			continue;
		if(DispMsg.cmd != LED_CMD_ON)
			continue;
		for(i = 0;i < DispMsg.len;i += 2) {
			led = DispMsg.pl[i];
			on  = DispMsg.pl[i + 1];
			if(led == LED_PL_DUMMY)
				continue;
			if(on)
				led_light_on(led);
			else
				led_light_off(led);
		}
	}
}

void vLed1DlyTask(void *pvParameters)
{
	char *name = (char *)pvParameters;
	TickType_t tick,i = 1;
	unsigned int led,sec;
	struct LedMsgFmt msg;
	BaseType_t r;

	for(;;) {
		r = xQueueReceive(Led1DlyQueue,(void *)&msg,portMAX_DELAY);
		vLedMsgPrint(&msg);
		if(r != pdPASS)
		if(r != pdPASS)
			continue;
		if(msg.cmd != LED_CMD_DLY)
			continue;
		led = msg.pl[0];
		sec = msg.pl[1];
		if((led != LED_PL_DUMMY) && (sec > 0)) {
			do {
				vTaskSuspendAll();
				rprintf("%s: %d\r\n",name,sec);
				xTaskResumeAll();

				tick = xTaskGetTickCount();
				vTaskDelayUntil(&tick,i);
				sec--;
			}while(sec != 0);

			xLedMsgPack(&msg,LED_CMD_RSP,LED_ACK_DLY_OK,0,0,0);
			r = xQueueSend(LedRspQueue,(void *)&msg,portMAX_DELAY);
			if(r != pdPASS) {
				vMsgPrint("vLed1DlyTask, send msg failed\r\n",'s');
			}
		}
	}
}

void vLed2DlyTask(void *pvParameters)
{
	char *name = (char *)pvParameters;
	TickType_t tick,i = 1;
	unsigned int led,sec;
	struct LedMsgFmt msg;
	BaseType_t r;

	for(;;) {
		r = xQueueReceive(Led2DlyQueue,(void *)&msg,portMAX_DELAY);
		vLedMsgPrint(&msg);
		if(r != pdPASS)
			continue;
		if(msg.cmd != LED_CMD_DLY)
			continue;
		led = msg.pl[2];
		sec = msg.pl[3];
		if((led != LED_PL_DUMMY) && (sec > 0)) {
			do {
				vTaskSuspendAll();
				rprintf("%s: %d\r\n",name,sec);
				xTaskResumeAll();

				tick = xTaskGetTickCount();
				vTaskDelayUntil(&tick,i);
				sec--;
			}while(sec != 0);

			xLedMsgPack(&msg,LED_CMD_RSP,0,LED_ACK_DLY_OK,0,0);
			r = xQueueSend(LedRspQueue,(void *)&msg,portMAX_DELAY);
			if(r != pdPASS) {
				vMsgPrint("vLed2DlyTask, send msg failed\r\n",'s');
			}
		}
	}
}

void vLed3DlyTask(void *pvParameters)
{
	char *name = (char *)pvParameters;
	TickType_t tick,i = 1;
	unsigned int led,sec;
	struct LedMsgFmt msg;
	BaseType_t r;

	for(;;) {
		r = xQueueReceive(Led3DlyQueue,(void *)&msg,portMAX_DELAY);
		vLedMsgPrint(&msg);
		if(r != pdPASS)
			continue;
		if(msg.cmd != LED_CMD_DLY)
			continue;
		led = msg.pl[4];
		sec = msg.pl[5];
		if((led != LED_PL_DUMMY) && (sec > 0)) {
			do {
				vTaskSuspendAll();
				rprintf("%s: %d\r\n",name,sec);
				xTaskResumeAll();

				tick = xTaskGetTickCount();
				vTaskDelayUntil(&tick,i);
				sec--;
			}while(sec != 0);

			xLedMsgPack(&msg,LED_CMD_RSP,0,0,LED_ACK_DLY_OK,0);
			r = xQueueSend(LedRspQueue,(void *)&msg,portMAX_DELAY);
			if(r != pdPASS) {
				vMsgPrint("vLed3DlyTask, send msg failed\r\n",'s');
			}
		}
	}
}

void vLedCtrlTask(void *pvParameters)
{
	TickType_t wait = 10;
	BaseType_t r;
	int err = 0,dly[] = {50,40,30,0},i = 0;
	struct LedMsgFmt MsgCtrl[3],MsgDly[3],MsgRsp;

	xLedMsgPack(&MsgCtrl[0],LED_CMD_ON,1,0,0,0);
	xLedMsgPack(&MsgCtrl[1],LED_CMD_ON,0,1,0,0);
	xLedMsgPack(&MsgCtrl[2],LED_CMD_ON,0,0,1,0);

	xLedMsgPack(&MsgDly[0],LED_CMD_DLY,dly[0],dly[1],dly[2],dly[3]);
	xLedMsgPack(&MsgDly[1],LED_CMD_DLY,dly[0],dly[1],dly[2],dly[3]);
	xLedMsgPack(&MsgDly[2],LED_CMD_DLY,dly[0],dly[1],dly[2],dly[3]);

	for(;;) {
		r = xQueueSend(LedDispQueue,(void *)&MsgCtrl[i],wait);
		if(r != pdPASS) {
			err = 0x01;
			break;
		}
		switch(i) {
		case 0:
			r = xQueueSend(Led1DlyQueue,(void *)&MsgDly[i],wait);
			break;
		case 1:
			r = xQueueSend(Led2DlyQueue,(void *)&MsgDly[i],wait);
			break;
		case 2:
			r = xQueueSend(Led3DlyQueue,(void *)&MsgDly[i],wait);
			break;
		default:
			break;
		}
		if(r != pdPASS) {
			err = 0x02;
			break;
		}
		r = xQueueReceive(LedRspQueue,(void *)&MsgRsp,portMAX_DELAY);
		vLedMsgPrint(&MsgRsp);
		if(r != pdPASS) {
			err = 0x04;
			break;
		}
		i++;
		i = i % 3;
	}
	vTaskSuspendAll();
	rprintf("%s, error %d\r\n",__func__,err);
	xTaskResumeAll();
	for(;;);
}

int xLedTaskConstructor(void)
{
	BaseType_t r;
	int depth = 4,stk = 512,width = sizeof(struct LedMsgFmt);

	rprintf("depth = %d, width = %d, stk = %d\r\n",depth,width,stk);
	// queues creattion
	LedDispQueue = xQueueCreate(depth,width);
	if(LedDispQueue == 0) {
		rprintf("%s, create LedDispQueue failed\n",__func__);
		return -1;
	}
	LedRspQueue = xQueueCreate(depth,width);
	if(LedRspQueue == 0) {
		rprintf("%s, create LedRspQueue failed\n",__func__);
		return -1;
	}
	Led1DlyQueue = xQueueCreate(depth,width);
	if(Led1DlyQueue == 0) {
		rprintf("%s, create Led1DlyQueue failed\n",__func__);
		return -1;
	}
	Led2DlyQueue = xQueueCreate(depth,width);
	if(Led2DlyQueue == 0) {
		rprintf("%s, create Led2DlyQueue failed\n",__func__);
		return -1;
	}
	Led3DlyQueue = xQueueCreate(depth,width);
	if(Led3DlyQueue == 0) {
		rprintf("%s, create Led3DlyQueue failed\n",__func__);
		return -1;
	}
	// tasks creattion
	r = xTaskCreate(vLedCtrlTask,"vLedCtrlTask",stk,NULL,2,&LedCtrlHandle);
	if(r != pdPASS) {
		rprintf("%s, create vLedCtrlTask failed\n",__func__);
		return -1;
	}
	r = xTaskCreate(vLedDispTask,"vLedDispTask",stk,NULL,2,&LedDispHandle);
	if(r != pdPASS) {
		rprintf("%s, create vLedDispTask failed\n",__func__);
		return -1;
	}
	r = xTaskCreate(vLed1DlyTask,"vLed1DlyTask",stk,"LED1 ",2,&Led1DlyHandle);
	if(r != pdPASS) {
		rprintf("%s, create vLed1DlyTask failed\n",__func__);
		return -1;
	}
	r = xTaskCreate(vLed2DlyTask,"vLed2DlyTask",stk,"LED2 ",2,&Led2DlyHandle);
	if(r != pdPASS) {
		rprintf("%s, create vLed2DlyTask failed\n",__func__);
		return -1;
	}
	r = xTaskCreate(vLed3DlyTask,"vLed3DlyTask",stk,"LED3 ",2,&Led3DlyHandle);
	if(r != pdPASS) {
		rprintf("%s, create vLed3DlyTask failed\n",__func__);
		return -1;
	}
	return 0;
}

u16 CalCRC16(u8 *pdata,int size)
{
	return 0xabcd;
}
