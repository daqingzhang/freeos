#ifndef __CORE_OP_H__
#define __CORE_OP_H__

unsigned int __get_XPSR(void);
unsigned int __get_APSR(void);
unsigned int __get_IPSR(void);
unsigned int __get_EPSR(void);
unsigned int __get_PRIMASK(void);
unsigned int __get_FAULTMASK(void);
unsigned int __get_BASEPRI(void);

void __set_APSR(unsigned int apsr);
void __set_PRIMASK(unsigned int mask);
void __set_FAULTMASK(unsigned int mask);
void __set_BASEPRI(unsigned int pri);
void __enable_irq(void);
void __disable_irq(void);

#endif
