#include <common.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <motor.h>

#define MTR_GPIOA	GPIOA
#define MTR_IN1		GPIO_Pin_4
#define MTR_IN2		GPIO_Pin_5
#define MTR_IN3		GPIO_Pin_6
#define MTR_IN4		GPIO_Pin_7

void motor_init(void)
{
	GPIO_InitTypeDef Init;
	unsigned int  pin = 0;

	//GPIOA
	pin = (MTR_IN1 | MTR_IN2 | MTR_IN3 | MTR_IN4);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	Init.GPIO_Pin = (unsigned short)pin;
	Init.GPIO_Mode = GPIO_Mode_Out_PP;
	Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MTR_GPIOA,&Init);

	GPIO_WriteBit(MTR_GPIOA,pin,Bit_RESET);
	rprintf("%s done!\r\n",__func__);
}

static u32 motor_dly = 500;

static void motor_step(int in1, int in2, int in3, int in4)
{
	GPIO_WriteBit(MTR_GPIOA,MTR_IN1,in1);
	GPIO_WriteBit(MTR_GPIOA,MTR_IN2,in2);
	GPIO_WriteBit(MTR_GPIOA,MTR_IN3,in3);
	GPIO_WriteBit(MTR_GPIOA,MTR_IN4,in4);
	mdelay(motor_dly);
}

static void motor_brake(void)
{
	motor_dly = 1;
	motor_step(0,0,0,0);
	motor_step(0,0,0,0);
	motor_step(0,0,0,0);
	motor_step(0,0,0,0);
}

void motor_rotate_clkwise(u32 dly)
{
	motor_dly = dly;
	motor_step(1,0,0,0);
	motor_step(0,0,1,0);
	motor_step(0,1,0,0);
	motor_step(0,0,0,1);
}

void motor_rotate_counter_clkwise(u32 dly)
{
	motor_dly = dly;
	motor_step(0,0,0,1);
	motor_step(0,1,0,0);
	motor_step(0,0,1,0);
	motor_step(1,0,0,0);
}

void motor_step8_rotate(u32 dly)
{
	motor_dly = dly;

	motor_step(1,0,0,0);
	motor_step(1,0,1,0);
	motor_step(0,0,1,0);
	motor_step(0,1,1,0);
	motor_step(0,1,0,0);
	motor_step(0,1,0,1);
	motor_step(0,0,0,1);
	motor_step(1,0,0,1);
}

void motor_test(void)
{
	u32 i,cnt = 50;
	u32 dly = 500;

	while(1) {

		for(i = 0;i < cnt;i++)
			//motor_rotate_clkwise(dly);
			//motor_rotate_counter_clkwise(dly);
			motor_step8_rotate(dly);
		motor_brake();
		break;
#if 0
		for(i = 0;i < cnt;i++)
			motor_rotate_counter_clkwise(dly);
		motor_brake();
#endif
	}
}
