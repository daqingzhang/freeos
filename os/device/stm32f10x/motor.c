#include <common.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <motor.h>

#define MTR_GPIOA	GPIOA
#define MTR_ENA		GPIO_Pin_4
#define MTR_IN1		GPIO_Pin_5
#define MTR_IN2		GPIO_Pin_6

#define MTR_GPIOB	GPIOB
#define MTR_ENB		GPIO_Pin_0
#define MTR_IN3		GPIO_Pin_1
#define MTR_IN4		GPIO_Pin_2

void motor_init(void)
{
	GPIO_InitTypeDef Init;
	unsigned int  pin = 0;

	//GPIOA
	pin = (MTR_ENA | MTR_IN1 | MTR_IN2);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	Init.GPIO_Pin = (unsigned short)pin;
	Init.GPIO_Mode = GPIO_Mode_Out_PP;
	Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MTR_GPIOA,&Init);

	GPIO_WriteBit(MTR_GPIOA,pin,Bit_RESET);

	//GPIOB
	pin = (MTR_ENB | MTR_IN3 | MTR_IN4);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	Init.GPIO_Pin = (unsigned short)pin;
	Init.GPIO_Mode = GPIO_Mode_Out_PP;
	Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MTR_GPIOB,&Init);

	GPIO_WriteBit(MTR_GPIOB,pin,Bit_RESET);
	rprintf("%s done!\r\n",__func__);
}

void sdelay(int sec)
{
	while(sec--) {
		mdelay(200);
		mdelay(200);
		mdelay(200);
		mdelay(200);
		mdelay(200);
	}
}

void motor_test(void)
{
	int i=500;
	u16 motor_pins[] = {MTR_IN1,MTR_IN2,MTR_IN3,MTR_IN4};
//	u16 motor_val1[] = {1,0,0,0};

	// enable
	GPIO_WriteBit(MTR_GPIOA,MTR_ENA,Bit_RESET);
	GPIO_WriteBit(MTR_GPIOB,MTR_ENB,Bit_RESET);
	while(1) {
		//EN1 = 1, EN2 = 0, EN3 = 0, EN4 = 0
		rprintf("phase 1\r\n");
		GPIO_WriteBit(MTR_GPIOA,motor_pins[0],1);
		GPIO_WriteBit(MTR_GPIOA,motor_pins[1],0);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[2],0);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[3],0);
		mdelay(i);

		rprintf("phase 2\r\n");
		GPIO_WriteBit(MTR_GPIOA,motor_pins[0],0);
		GPIO_WriteBit(MTR_GPIOA,motor_pins[1],1);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[2],0);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[3],0);
		mdelay(i);
#if 1
		rprintf("phase 3\r\n");
		GPIO_WriteBit(MTR_GPIOA,motor_pins[0],0);
		GPIO_WriteBit(MTR_GPIOA,motor_pins[1],0);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[2],1);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[3],0);
		mdelay(i);
	
		rprintf("phase 4\r\n");
		GPIO_WriteBit(MTR_GPIOA,motor_pins[0],0);
		GPIO_WriteBit(MTR_GPIOA,motor_pins[1],0);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[2],0);
		GPIO_WriteBit(MTR_GPIOB,motor_pins[3],1);
		mdelay(i);
#endif
	}
}
