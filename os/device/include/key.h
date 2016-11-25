#ifndef __KEY_H__
#define __KEY_H__

//#define CONFIG_DEBUG_KEY

#define KEY2_ID	(1<<0)
#define KEY3_ID	(1<<1)
#define KEY4_ID	(1<<2)

int key_init(int key);
unsigned int read_key(void);

#endif
