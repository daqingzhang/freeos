#ifndef __LEDTASK_H__
#define __LEDTASK_H__
#include <common.h>

#define LED_MSG_HEAD_SIZE	1
#define LED_MSG_CMD_SIZE	1
#define LED_MSG_LEN_SIZE	2
#define LED_MSG_PL_SIZE		16
#define LED_MSG_CRC_SIZE	2

#define LED_MSG_SIZE	(LED_MSG_HEAD_SIZE + LED_MSG_CMD_SIZE	\
			+ LED_MSG_LEN_SIZE + LED_MSG_PL_SIZE + LED_MSG_CRC_SIZE)

#define LED_MSG_HEAD 0x45
#define LED_PL_DUMMY 0xff

enum LedMsgCmdType
{
	LED_CMD_NULL= 0xff,
	LED_CMD_ON  = 0x31,
	LED_CMD_DLY = 0x32,
	LED_CMD_RSP = 0x33,
};

enum LedMsgPLLType
{
	LED_PLL_ON = 6,
	LED_PLL_DLY = 6,
	LED_PLL_RSP = 6,
};

enum LedMsgAckType
{
	LED_ACK_DLY_OK = 0x51,
};

struct LedMsgFmt
{
	u8  head;
	u8  cmd;
	u16 len;
	u8  pl[LED_MSG_PL_SIZE];
	u16 crc;
};

extern TaskHandle_t LedDispHandle;
extern TaskHandle_t Led1DlyHandle;
extern TaskHandle_t Led2DlyHandle;
extern TaskHandle_t Led3DlyHandle;
extern TaskHandle_t LedCtrlHandle;

extern QueueHandle_t LedCmdQueue;
extern QueueHandle_t LedRspQueue;
extern QueueHandle_t Led1DlyQueue;
extern QueueHandle_t Led2DlyQueue;
extern QueueHandle_t Led3DlyQueue;

int xLedTaskConstructor(void);

#endif /* __LEDTASK_H__ */
