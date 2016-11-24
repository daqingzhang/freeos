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

#define LED_FSM_R0 0
#define LED_FSM_R1 1
#define LED_FSM_R2 2
#define LED_FSM_R3 3
#define LED_FSM_R4 4
#define LED_FSM_R5 5
#define LED_FSM_R6 6
#define LED_FSM_R7 7
#define LED_FSM_R8 8

#define LED_FSM_S0 0
#define LED_FSM_S1 1
#define LED_FSM_S2 2
#define LED_FSM_S3 3

enum LedFsmRouteType
{
	DISP_TO_DISP	= LED_FSM_R0,
	DISP_TO_DLY	= LED_FSM_R1,
	DLY_TO_DLY	= LED_FSM_R2,
	DLY_TO_RSP	= LED_FSM_R3,
	RSP_TO_RSP	= LED_FSM_R4,
	RSP_TO_DISP	= LED_FSM_R5,
	RSP_TO_IDLE	= LED_FSM_R6,
	IDLE_TO_IDLE	= LED_FSM_R7,
	IDLE_TO_DISP	= LED_FSM_R8,
};

enum LedFsmStateType
{
	STATE_DISP	= LED_FSM_S0,
	STATE_DLY	= LED_FSM_S1,
	STATE_RSP	= LED_FSM_S2,
	STATE_IDLE	= LED_FSM_S3,
};

struct LedMsgStateType
{
	enum LedFsmStateType current;
	enum LedFsmRouteType r;
	enum LedFsmStateType next;
};

int xLedMsgStateUpdate(struct LedMsgStateType *state, enum LedFsmRouteType route);
int xLedMsgStateSwitch(struct LedMsgStateType *state);
int xLedMsgCurStateGet(struct LedMsgStateType *state);
int xLedMsgNextStateGet(struct LedMsgStateType *state);
int xLedMsgStateInit(struct LedMsgStateType *p);
int xLedMsgStatePrint(struct LedMsgStateType *p);

extern TaskHandle_t LedDispHandle;
extern TaskHandle_t Led1DlyHandle;
extern TaskHandle_t Led2DlyHandle;
extern TaskHandle_t Led3DlyHandle;
extern TaskHandle_t LedCtrlHandle;

extern QueueHandle_t LedDispQueue;
extern QueueHandle_t LedRspQueue;
extern QueueHandle_t Led1DlyQueue;
extern QueueHandle_t Led2DlyQueue;
extern QueueHandle_t Led3DlyQueue;

int xLedTaskConstructor(void);

#endif /* __LEDTASK_H__ */
