#include <timer.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_system.h>
#include <stm32f10x_rcc.h>

void timer_init(int id)
{
	if(id == TIMER4_ID)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	else if(id == TIMER3_ID)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	else if(id == TIMER2_ID)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	else if(id == TIMER1_ID)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	else
		return;
}

void timer_delay_us(int id, int us)
{
	TIM_TypeDef *tim;
	u32 pscl = (clktree.apb1clk) / 1000000;

	if(id == TIMER4_ID)
		tim = TIM4;
	else if(id == TIMER3_ID)
		tim = TIM3;
	else if(id == TIMER2_ID)
		tim = TIM2;
	else if(id == TIMER1_ID)
		tim = TIM1;
	else
		tim = 0;
	if(!tim)
		return;

	//downcounter,edge aligned
	tim->CR1 = TIM_CR1_DIR | TIM_CR1_CMS;

	if(us <= 0xffff) {
		tim->PSC = (u16)pscl;
		tim->ARR = us;
	} else {
		tim->PSC = (u16)pscl << 2;
		tim->ARR = us >> 2;
	}
	tim->EGR = TIM_EGR_UG;
	//clear flag
	tim->SR = ~(TIM_SR_UIF);
	//run timer
	tim->CR1 |= TIM_CR1_CEN;
	while((tim->SR & TIM_SR_UIF) == 0);
	tim->CR1 &= ~TIM_CR1_CEN;
	//clear flag
	tim->SR = ~(TIM_SR_UIF);
}

int timer_capture_pulse(int id, unsigned int *period, unsigned int *pos_width, int enable)
{
	return 0;
}

int timer_output_pulse(int id, unsigned int period, unsigned int pos_width,int enable)
{
	return 0;
}

void udelay(unsigned long us)
{
	timer_delay_us(DELAY_TIMER,us);
}

void mdelay(unsigned long ms)
{
	timer_delay_us(DELAY_TIMER,ms * 1000);
}

