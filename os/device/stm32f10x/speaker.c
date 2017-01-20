#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <common.h>
#include <speaker.h>

#define SPK_GPIOX	GPIOC
#define SPK_PIN		GPIO_Pin_4
#define SPK_BEEP_DLY_MS 100

void speaker_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	unsigned int  pin = SPK_PIN;

	/* enable GPIOC clock, GPIOC_4 pin */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = (unsigned short)pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPK_GPIOX,&GPIO_InitStructure);

	// speaker output low level
	GPIO_WriteBit(SPK_GPIOX,pin,Bit_RESET);

	rprintf("%s done!\r\n",__func__);
}

void speaker_on(void)
{
	GPIO_WriteBit(SPK_GPIOX,SPK_PIN,Bit_SET);
}

void speaker_off(void)
{
	GPIO_WriteBit(SPK_GPIOX,SPK_PIN,Bit_RESET);
}

void speaker_beep(int times, unsigned int dly)
{
	unsigned int us = dly;

	if(!us)
		us = SPK_BEEP_DLY_MS;
	while(times--) {
		speaker_on();
		udelay(us);

		speaker_off();
		udelay(us);
	}
}

void speaker_test(void)
{
	rprintf("%s start!\r\n",__func__);
	speaker_beep(10000, 200);
}
