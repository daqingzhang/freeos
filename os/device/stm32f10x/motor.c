#include <common.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <motor.h>

#define MTR_GPIOA	GPIOA
#define MTR_IN1		GPIO_Pin_4
#define MTR_IN2		GPIO_Pin_5
#define MTR_IN3		GPIO_Pin_6
#define MTR_IN4		GPIO_Pin_7

static void motor_step(int in1, int in2, int in3, int in4);

void motor_init(void)
{
	unsigned int  pin = 0;
	GPIO_InitTypeDef Init;

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

void motor_brake(void)
{
	motor_step(0,0,0,0);
	motor_step(0,0,0,0);
	motor_step(0,0,0,0);
	motor_step(0,0,0,0);
}

static void motor_step(int in1, int in2, int in3, int in4)
{
	GPIO_WriteBit(MTR_GPIOA,MTR_IN1,in1);
	GPIO_WriteBit(MTR_GPIOA,MTR_IN2,in2);
	GPIO_WriteBit(MTR_GPIOA,MTR_IN3,in3);
	GPIO_WriteBit(MTR_GPIOA,MTR_IN4,in4);
}

static void motor_rotate_step4(int clockwise)
{
	if(clockwise) {
		motor_step(1,0,0,0);
		motor_step(0,0,1,0);
		motor_step(0,1,0,0);
		motor_step(0,0,0,1);
	} else {
		motor_step(0,0,0,1);
		motor_step(0,1,0,0);
		motor_step(0,0,1,0);
		motor_step(1,0,0,0);
	}
}

static void motor_rotate_step8(int clockwise)
{
	if(clockwise) {
		motor_step(1,0,0,0);
		motor_step(1,0,1,0);
		motor_step(0,0,1,0);
		motor_step(0,1,1,0);
		motor_step(0,1,0,0);
		motor_step(0,1,0,1);
		motor_step(0,0,0,1);
		motor_step(1,0,0,1);
	} else {
		motor_step(1,0,0,1);
		motor_step(0,0,0,1);
		motor_step(0,1,0,1);
		motor_step(0,1,0,0);
		motor_step(0,1,1,0);
		motor_step(0,0,1,0);
		motor_step(1,0,1,0);
		motor_step(1,0,0,0);
	}
}

void motor_rotate(int step8, int clockwise)
{
	if(step8)
		motor_rotate_step8(clockwise);
	else
		motor_rotate_step4(clockwise);
}

void motor_test(void)
{
	u32 i,cnt = 50;
	u32 us = 500;

	while(1) {

		for(i = 0;i < cnt;i++) {
			motor_rotate(1,1);
			udelay(us);
		}
		motor_brake();
		break;
	}
}
