#ifndef __COMMON_H__
#define __COMMON_H__
#include <base_types.h>
#include <core_op.h>

#define core_irq_enable  __enable_irq
#define core_irq_disable __disable_irq

void printf(char cmd, ...);

void mdelay(unsigned long ms);
void udelay(unsigned long us);

#endif
