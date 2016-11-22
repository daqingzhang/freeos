#include <stm32f10x_system.h>

#define AIRCR_VECTKEY_MASK 0x05FA0000
#define CONFIG_CLKSRC_TIMEOUT 1000

struct clock_tree clktree = {
	.sysclk = HSE_VALUE,
	.apb1clk = HSE_VALUE,
	.apb2clk = HSE_VALUE,
};

static int set_system_clock(int clk);

u32 system_get_cpuid(void)
{
	return SCB->CPUID;
}

void system_nmi_set_pending(void)
{
	SCB->ICSR |= SCB_ICSR_NMIPENDSET_Msk;
}

void system_svc_set_pending(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void system_svc_clr_pending(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
}

void system_systick_set_pending(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;
}

void system_systick_clr_pending(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
}

void system_softreset(void)
{
	__DSB();
	__ISB();
	SCB->AIRCR |= SCB_AIRCR_VECTRESET_Msk;
	while(1);
}

int system_set_priority_group(u32 grp)
{
	grp &= (0x7 << 8);
	SCB->AIRCR = AIRCR_VECTKEY_MASK | grp;
	return 0;
}

int system_set_vectaddr(u32 base, u32 offset)
{
	SCB->VTOR = base | (offset & 0x1FFFFF80);
	return 0;
}

int system_set_low_power(u32 lp_mode, int lp_enable)
{
	if(lp_enable)
		SCB->SCR |= lp_mode;
	else
		SCB->SCR &= ~lp_mode;
	return 0;
}


int system_nvic_set_channel(u32 id, u32 prio_preempt, u32 prio_sub, int enable)
{
	u32 reg_id = id >> 5;
	u32 mask   = 1 << (id & 0x1f);
	u32 grp,prio;

	if(!enable) {
		NVIC->ICER[reg_id] = mask;
	} else {
		grp = SCB->AIRCR & 0x700;
		if(grp == NVIC_PRIORITY_GRP0) {
			/* preempt = 0 bits, subpriority = 4 bits */
			prio = prio_sub & 0xf;
		} else if(grp == NVIC_PRIORITY_GRP1) {
			/* preempt = 1 bits, subpriority = 3 bits */
			prio = ((prio_preempt & 0x1) << 3) | (prio_sub & 0x7);
		} else if(grp == NVIC_PRIORITY_GRP2) {
			/* preempt = 2 bits, subpriority = 2 bits */
			prio = ((prio_preempt & 0x3) << 2) | (prio_sub & 0x3);
		} else if(grp == NVIC_PRIORITY_GRP3) {
			/* preempt = 3 bits, subpriority = 1 bits */
			prio = ((prio_preempt & 0x7) << 1) | (prio_sub & 0x1);
		} else {
			/* preempt = 4 bits, subpriority = 0 bits */
			prio = prio_preempt & 0xf;
		}
		NVIC->IP[id] = prio;
		NVIC->ICER[reg_id] = mask;
	}
	return 0;
}

int system_systick_config(u32 ticks)
{
	if(ticks > SysTick_LOAD_RELOAD_Msk)
		return -1;

	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->LOAD = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;
	// set SysTick priority = 15
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS) - 1);
	// clear flags
	SysTick->VAL  = 0;
	// enable IRQ, use processor clock
	SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk);
	return 0;
}

void system_systick_run(int yes)
{
	if(yes)
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	else
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

int system_init_clock(void)
{
	// HSION = 1, enable HSI clock
	RCC->CR |= RCC_CR_HSION;

	//clear ADCPRE,APB2PRE,APB1PRE,AHBPRE,SWS,SW,MCO bits
	RCC->CFGR &= ~((0x7 << 24) | 0xFFFF);

	//HSEON=0(bit[16]),CSSON=0(bit[19]),PLLON=0(bit[24])
	RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);
	//HSEBYP=0
	RCC->CR &= ~(RCC_CR_HSEBYP);

	//clear PLLSRC,PLLXTPRE,PLLMUL,USBPRE bits
	RCC->CR &= ~(0x7F << 16);

	//clear pending flags & disable interrupts
	RCC->CIR = ((1 << 23) | (0x1F << 16));

	return(set_system_clock(CONFIG_SYSCLK));
}

static int set_system_clock(int clk)
{
	int i = CONFIG_CLKSRC_TIMEOUT;
	u32 mul = 0;

	// HSEON = 1,enable HSE clock
	RCC->CR |= RCC_CR_HSEON;

	do {
		__NOP();
		--i;
		if(RCC->CR & RCC_CR_HSERDY)
			break;
	}while(i > 0);

	if(!(RCC->CR & RCC_CR_HSERDY))
		return -1;

	// config flash memory
	FLASH->ACR |= FLASH_ACR_PRFTBE;
	FLASH->ACR &= ~(FLASH_ACR_LATENCY);
	FLASH->ACR |= FLASH_ACR_LATENCY_2;

	// config AHB bus frequency divider,HCLK = SYSCLK/1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	// config APB2 bus frequency divider,APB2CLK = HCLK/2
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	// config APB1 bus frequency divider,APB1CLK = HCLK/2
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	switch(clk) {
	case CONFIG_SYSCLK_72MHZ:
		mul = RCC_CFGR_PLLMULL9;
		clktree.sysclk  = CONFIG_SYSCLK_72MHZ;
		clktree.apb1clk = CONFIG_APBCLK_36MHZ;
		clktree.apb2clk = CONFIG_APBCLK_36MHZ;
		break;
	case CONFIG_SYSCLK_64MHZ:
	default:
		mul = RCC_CFGR_PLLMULL8;
		clktree.sysclk  = CONFIG_SYSCLK_64MHZ;
		clktree.apb1clk = CONFIG_APBCLK_32MHZ;
		clktree.apb2clk = CONFIG_APBCLK_32MHZ;
		break;
	}
	// config PLLCLK = HSE * mul;
	RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | mul);

	// enable PLL
	RCC->CR |= RCC_CR_PLLON;

	i = CONFIG_CLKSRC_TIMEOUT;
	do {
		__NOP();
		--i;
		if(RCC->CR & RCC_CR_PLLRDY)
			break;
	} while(i > 0);

	if(!(RCC->CR & RCC_CR_PLLRDY))
		return -1;

	// switch PLL as SYSCLK source
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

	return 0;
}

