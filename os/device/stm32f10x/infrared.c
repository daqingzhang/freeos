#include <common.h>
#include <infrared.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#define INFRARED_GPIO	GPIOB
#define INFRARED_OUT	GPIO_Pin_0
#define INFRARED_MASK	0x0001

void infrared_init(void)
{
	unsigned int pin = 0;
	GPIO_InitTypeDef Init;

	pin = GPIO_Pin_0;

	//GPIOB0, input
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	Init.GPIO_Pin = (unsigned short)pin;
	Init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(INFRARED_GPIO,&Init);

	rprintf("%s done!\r\n",__func__);
}

int infrared_get_value(void)
{
	u16 t;

	t = INFRARED_MASK & GPIO_ReadInputData(INFRARED_GPIO);
	return t;
}

void infrared_test(void)
{
	int val = 0;
	while(1) {
		mdelay(500);
		val = infrared_get_value();
		rprintf("%s, val = %d\r\n",__func__,val);
	}
}
