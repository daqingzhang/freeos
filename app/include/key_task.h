#ifndef __KEY_TASK_H__
#define __KEY_TASK_H__

extern TaskHandle_t KeyGetHandle;
extern TaskHandle_t KeyPrcHandle;
extern TaskHandle_t KeyWthHandle;

int xKeyTaskConstructor(void);
#endif
