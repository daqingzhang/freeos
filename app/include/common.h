#ifndef __COMMON_H__
#define __COMMON_H__
#include <core_op.h>
#include <serial.h>
#include <timer.h>

void rprintf(const char *fmt, ...);
void mdelay(unsigned long ms);
void udelay(unsigned long us);

#endif
