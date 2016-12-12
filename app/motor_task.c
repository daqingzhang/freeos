#include <common.h>
#include <string.h>
#include <motor.h>
#include <infrared.h>
#include <motor_task.h>

TaskHandle_t InfraredSensorHandle;
TaskHandle_t MotorDriveHandle;
TaskHandle_t MotorControlHandle;

QueueHandle_t InfraredSensorQueue;
QueueHandle_t MotorDriveQueue;
QueueHandle_t MotorControlQueue;

#define CONFIG_DEBUG
#ifdef CONFIG_DEBUG
#define dprintf rprintf
#else
#define dprintf(...) do{}while(0)
#endif

unsigned short CheckSum(unsigned short *pd,int len)
{
	u32 sum = 0;

	while(len > 1) {
		sum += *pd++;
		len -= sizeof(unsigned short);
	}
	if(len)
		sum += *(u8 *)(pd);

	sum = (sum >> 16) + (sum & 0xffff);

	return (unsigned short)(~sum);
}

int xAutoMobileMsgPack(struct AMMessage *msg, u8 id, u8 *pl, u16 len)
{
	u16 cs;

	if(msg == NULL)
		return -AMRET_ERR_NULL_PTR;
	if(len >= AMM_MAX_LEN)
		return -AMRET_ERR_INV_LEN;

	switch(id) {
	case AMMID_BLOCK:
	case AMMID_BRAKE:
	case AMMID_HOLD:
	case AMMID_SPEED:
	case AMMID_DIRECTION:
		msg->head = AMM_HEAD;
		msg->id	  = id;
		msg->len  = len;
		memcpy(msg->pl,pl,len);
		memset(msg->pl+ len,AMM_DUMMY,AMM_MAX_LEN - len);
		msg->sum = 0x0;
		cs = CheckSum((unsigned int *)msg,sizeof(struct AMMessage));
		msg->sum = cs;
		break;
	default:
		return -AMRET_ERR_INV_ID;
		break;
	}
	return 0;
}

int xAutoMobileMsgUnpack(struct AMMessage *msg,u8 *id, u8 *pl, u16 *len)
{
	u16 cs = 0;

	if(msg == NULL)
		return -AMRET_ERR_NULL_PTR;
	if(len >= AMM_MAX_LEN)
		return -AMRET_ERR_INV_LEN;

	switch(id) {
	case AMMID_BLOCK:
	case AMMID_BRAKE:
	case AMMID_HOLD:
	case AMMID_SPEED:
	case AMMID_DIRECTION:
		*id = msg->id;
		*len = msg->len;
		cs = CheckSum((unsigned int *)msg,sizeof(struct AMMessage));
		if(cs != 0) {
			dprintf("%s, err checksum = %x\n",__func__,cs);
			return -AMRET_ERR_CHECKSUM;
		}
		memcpy(pl,msg->pl,*len);
		break;
	default:
		return -AMRET_ERR_INV_ID;
		break;
	}
	return 0;
}

void vInfraredSensorTask(void *pvParameters)
{
	TickType_t tick = 0;
	u8 block[2],err,msg[50];
	struct AMMessage msg;

	for(;;) {
		tick = xTaskGetTickCount();
		vTaskDelayUntil(&tick,2);

		block[0] = infrared_is_blocked(0);
		block[1] = infrared_is_blocked(1);
		err = xAutoMobileMsgPack(&msg, AMMID_BLOCK,block,2);
		if(err) {
			dprintf("%s, pack msg error, %d\n",__func__,err);
			continue;
		}
		xQueueSend(InfraredSensorQueue,(void *)&msg,portMAX_DELAY);
	}
}

void vMotorDriveTask(void *pvParameters)
{
	u8 id = 0,temp[AMM_MAX_LEN];
	u16 len = 0;
	int r;
	BaseType_t rx;
	struct AMMessage msg;

	for(;;) {
		rx = xQueueReceive(MotorDriveQueue,(void *)&msg,portMAX_DELAY);
		if(rx == != pdPASS)
			continue;
		r = xAutoMobileMsgUnpack(&msg,&id,temp,&len);
		if(r) {
			dprintf("%s, unpack msg error, %d\r\n",__func__,r);
			continue;
		}
		switch(id) {
		case AMMID_BRAKE:
			break;
		case AMMID_HOLD:
			break;
		case AMMID_SPEED:
			break;
		case AMMID_DIRECTION:
			break;
		default:
			break;
		}
	}
}

void vMotorControlTask(void *pvParameters)
{

}

int vCreateFailed(int err)
{
	rprintf("task create failed, %d\r\n",err);
	return -err;
}

int xAMTaskConstructor(void)
{
	BaseType_t r;
	int stk = 256,depth = 50;
	int width = sizeof(struct AMMessage);

	InfraredSensorQueue = xQueueCreate(depth,width);
	MotorDriveQueue	    = xQueueCreate(depth,width);
	MotorControlQueue   = xQueueCreate(depth,width);

	r = xTaskCreate(vInfraredSensorTask,"vInfraredSensorTask",stk,NULL,2,&InfraredSensorTask);
	if(r != pdPASS) {
		return vCreateFailed(1);
	}
	r = xTaskCreate(vMotorDriveTask,"vMotorDriveTask",stk,NULL,2,&MotorDriveTask);
	if(r != pdPASS) {
		return vCreateFailed(2);
	}
	r = xTaskCreate(vMotorControlTask,"vMotorControlTask",stk,NULL,2,&MotorControlTask);
	if(r != pdPASS) {
		return vCreateFailed(3);
	}
}

