#ifndef __SERIAL_H__
#define __SERIAL_H__

#define CONFIG_USART1_BAUDRATE	19200
#define CONFIG_USART2_BAUDRATE	19200

#define NVIC_CHANNEL_PRIO_USART1 13
#define NVIC_CHANNEL_PRIO_USART2 12

enum {
	NULL_ID   = 0,
	USART1_ID = 1,
	USART2_ID = 2,
};

void serial_init(int id);
int  serial_tstc(int id);
char serial_getc(int id);
void serial_putc(int id,char c);
void serial_puts(int id,const char *s);
int  serial_gets(int id,char *s);
#endif
