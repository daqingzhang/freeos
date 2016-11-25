#include <string.h>
#include <key.h>
#include <timer.h>
#include <serial.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

#define KEY_GPIO GPIOB
#define K2_PIN GPIO_Pin_9
#define K3_PIN GPIO_Pin_8
#define K4_PIN GPIO_Pin_7

#define KEY_PIN_MASK (K2_PIN | K3_PIN | K4_PIN)

int key_init(int key)
{
	u16 pin = 0;
	GPIO_InitTypeDef Init;

	if(key & KEY2_ID)
		pin |= K2_PIN;
	if(key & KEY3_ID)
		pin |= K3_PIN;
	if(key & KEY4_ID)
		pin |= K4_PIN;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	Init.GPIO_Pin 	= pin;
	Init.GPIO_Mode 	= GPIO_Mode_IPU;
	Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_GPIO,&Init);
#ifdef CONFIG_DEBUG_KEY
	rprintf("%s, CRL = %x,CRH = %x\r\n",__func__,KEY_GPIO->CRL,GPIOB->CRH);
#endif
	return 0;
}

unsigned int read_key(void)
{
	u32 t = 0;

	t = GPIO_ReadInputData(KEY_GPIO);
#ifdef CONFIG_DEBUG_KEY
	rprintf("%s, value: %x\r\n",__func__,t);
#endif
	// pressed = 0,unpressed = 1
	t = (KEY_PIN_MASK & (~t)) >> 7;
	return t;
}
