#ifndef __COMMON_H__
#define __COMMON_H__
#include <core_op.h>
#include <serial.h>
#include <timer.h>

#define ARRAY_SIZE(arr)	(sizeof(arr)/sizeof((arr)[0]))

#ifdef CONFIG_USE_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

void vMsgPrint(void  *pvParameters,char c);
u16 CalCRC16(u8 *pdata,int size);
#endif

void rprintf(const char *fmt, ...);
void mdelay(unsigned long ms);
void udelay(unsigned long us);

#endif
