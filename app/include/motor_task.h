#ifndef __MOTOR_TASK_H__
#define __MOTOR_TASK_H__

#define AMM_HEAD	0x5A
#define AMM_MAX_LEN	16
#define AMM_DUMMY	0xff

enum AUTOMOBILE_RET {
	AMRET_ERR_NONE = 0,
	AMRET_ERR_NULL_PTR = 0xE0,
	AMRET_ERR_INV_ID = 0xE1,
	AMRET_ERR_INV_LEN = 0xE2,
	AMRET_ERR_CHECKSUM = 0xE3,
};

enum AUTOMOBILE_MSG_ID {
	AMMID_BLOCK = 0x31,
	AMMID_BRAKE = 0x32,
	AMMID_HOLD = 0x33,
	AMMID_SPEED = 0x34,
	AMMID_DIRECTION = 0x35,
};

struct AMMessage {
	u8	head;
	u8	id;
	u16	len;
	u8	pl[AMM_MAX_LEN];
	u32	sum;
};

#endif
