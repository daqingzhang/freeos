#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <common.h>
#include <led.h>

#define LED_GPIOX 	GPIOC
#define LED1_PIN 	GPIO_Pin_3
#define LED2_PIN 	GPIO_Pin_1
#define LED3_PIN 	GPIO_Pin_13
#define LED_PIN_ALL 	(LED1_PIN | LED2_PIN | LED3_PIN)

void led_init(unsigned int led)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	unsigned int  pin = 0;

	if(led & LED1)
		pin |= LED1_PIN;
	if(led & LED2)
		pin |= LED2_PIN;
	if(led & LED3)
		pin |= LED3_PIN;
	/*
	 * enable GPIOC clock, GPIOC_3, GPIOC_1, GPIOC_13 is LED pins
	 */
	//GPIO_DeInit(LED_GPIOX);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = (unsigned short)pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIOX,&GPIO_InitStructure);

	// set led light off
	GPIO_WriteBit(GPIOC,pin,Bit_SET);
}

void led_on(unsigned int led)
{
	if(led & LED1)
		GPIO_WriteBit(LED_GPIOX,LED1_PIN,Bit_RESET);
	if(led & LED2)
		GPIO_WriteBit(LED_GPIOX,LED2_PIN,Bit_RESET);
	if(led & LED3)
		GPIO_WriteBit(LED_GPIOX,LED3_PIN,Bit_RESET);
}

void led_off(unsigned int led)
{
	if(led & LED1)
		GPIO_WriteBit(LED_GPIOX,LED1_PIN,Bit_SET);
	if(led & LED2)
		GPIO_WriteBit(LED_GPIOX,LED2_PIN,Bit_SET);
	if(led & LED3)
		GPIO_WriteBit(LED_GPIOX,LED3_PIN,Bit_SET);
}

void led_flash(unsigned int led, unsigned int on,unsigned int off)
{
	led_on(led);
	mdelay(on);

	led_off(led);
	mdelay(off);
}
