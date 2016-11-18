#ifndef __COMMON_H__
#define __COMMON_H__
#include <base_types.h>
#include <core_op.h>
#include <serial.h>

void rprintf(const char *fmt, ...);
void mdelay(unsigned long ms);
void udelay(unsigned long us);

#endif
