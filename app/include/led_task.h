#ifndef __LEDTASK_H__
#define __LEDTASK_H__
#include <common.h>
#include <fsm.h>

#define LED_MSG_HEAD_SIZE	1
#define LED_MSG_CMD_SIZE	1
#define LED_MSG_LEN_SIZE	2
#define LED_MSG_PL_SIZE		16
#define LED_MSG_CRC_SIZE	2
#define LED_MSG_SIZE	(LED_MSG_HEAD_SIZE + LED_MSG_CMD_SIZE	\
			+ LED_MSG_LEN_SIZE + LED_MSG_PL_SIZE + LED_MSG_CRC_SIZE)

#define LED_MSG_HEAD 0xAA
#define LED_PL_DUMMY 0xFF

enum LedMsgCmdType
{
	LED_CMD_NULL= 0xff,
	LED_CMD_ON  = 0x31,
	LED_CMD_DLY = 0x32,
	LED_CMD_RSP = 0x33,
};

enum LedMsgPLLType
{
	LED_PLL_ON  = 6,
	LED_PLL_DLY = 6,
	LED_PLL_RSP = 6,
};

enum LedMsgAckType
{
	LED_ACK_DLY_OK = 0x51,
};

/*
 * Led Message Formation
 * -----------------------------------
 * | head | cmd | len | payload |crc |
 * -----------------------------------
 * | 1B   | 1B  | 2B  | 16B     | 2B |
 * -----------------------------------
 *
 * Tasks uses several messages for conresponding:
 * (1) Led_On_Off_Msg:
 * 	----------------------------------------------------------
 *	| LED_MSG_HEAD | LED_CMD_ON | LED_PLL_ON | payload | CRC |
 *	----------------------------------------------------------
 *
 * (2) Led_Delay_Msg:
 * 	----------------------------------------------------------
 *	| LED_MSG_HEAD | LED_CMD_DLY| LED_PLL_DLY| payload | CRC |
 *	----------------------------------------------------------
 *
 * (3) Led_Response_Msg:
 * 	----------------------------------------------------------
 *	| LED_MSG_HEAD | LED_CMD_RSP| LED_PLL_RSP| payload | CRC |
 *	----------------------------------------------------------
 *
 * The Payload field data:
 * 	----------------------------------------------------------------------
 *	| led1_id | data1 | led2_id | data2  | led3_id | data3 | 0xff...0xff |
 *	----------------------------------------------------------------------
 * Each message's length is fixed.
 */

struct LedMsgFmt
{
	u8  head;
	u8  cmd;
	u16 len;
	u8  pl[LED_MSG_PL_SIZE];
	u16 crc;
};

#define DISP_TO_DISP	FSM_COMM_R0
#define DISP_TO_DLY	FSM_COMM_R1
#define DLY_TO_DLY	FSM_COMM_R2
#define DLY_TO_RSP	FSM_COMM_R3
#define RSP_TO_RSP	FSM_COMM_R4
#define RSP_TO_DISP	FSM_COMM_R5
#define RSP_TO_IDLE	FSM_COMM_R6
#define IDLE_TO_IDLE	FSM_COMM_R7
#define IDLE_TO_DISP	FSM_COMM_R8

#define STATE_DISP	FSM_COMM_S0
#define STATE_DLY	FSM_COMM_S1
#define STATE_RSP	FSM_COMM_S2
#define STATE_IDLE	FSM_COMM_S3

struct fsm_device *led_fsm_get_device(void);

int xLedTaskConstructor(void);

extern TaskHandle_t LedDispHandle;
extern TaskHandle_t Led1DlyHandle;
extern TaskHandle_t Led2DlyHandle;
extern TaskHandle_t Led3DlyHandle;
extern TaskHandle_t LedCtrlHandle;

#if 0
extern QueueHandle_t LedDispQueue;
extern QueueHandle_t LedRspQueue;
extern QueueHandle_t Led1DlyQueue;
extern QueueHandle_t Led2DlyQueue;
extern QueueHandle_t Led3DlyQueue;
#endif

#endif /* __LEDTASK_H__ */
