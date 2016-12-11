#define CONFIG_STACK_INIT_SP 0x20005000

	.syntax	unified
	.arch armv7-m
	.cpu cortex-m3
	.thumb

	.align 2
	.globl _start
_start:
	.long	CONFIG_STACK_INIT_SP  /* Top of Stack 	*/
	.long	Reset_Handler         /* Reset Handler 	*/
	.long	NMI_Handler           /* NMI Handler 	*/
	.long	HardFault_Handler     /* Hard Fault Handler 	*/
	.long	MemManage_Handler     /* MPU Fault Handler 	*/
	.long	BusFault_Handler      /* Bus Fault Handler 	*/
	.long	UsageFault_Handler    /* Usage Fault Handler 	*/
	.long	0                     /* Reserved 	*/
	.long	0                     /* Reserved 	*/
	.long	0                     /* Reserved 	*/
	.long	0                     /* Reserved 	*/
	.long	SVC_Handler           /* SVCall Handler */
	.long	DebugMon_Handler      /* Debug Monitor Handler */
	.long	0                     /* Reserved 	*/
	.long	PendSV_Handler        /* PendSV Handler */
	.long	SysTick_Handler       /* SysTick Handler*/
	.long	Default_Handler /* WWDOG_Handler	*/
	.long	Default_Handler /* PVD_Handler		*/
	.long	Default_Handler /* TAMPER_Handler	*/
	.long	Default_Handler /* RTC_Handler		*/
	.long	Default_Handler /* FLASH_Handler	*/
	.long	Default_Handler /* RCC_Handler		*/
	.long	Default_Handler /* EXTI0_Handler	*/
	.long	Default_Handler /* EXTI1_Handler	*/
	.long	Default_Handler /* EXTI2_Handler	*/
	.long	Default_Handler /* EXTI3_Handler	*/
	.long	Default_Handler /* EXTI4_Handler	*/
	.long	Default_Handler /* DMA1_CH1_Handler	*/
	.long	Default_Handler /* DMA1_CH2_Handler	*/
	.long	Default_Handler /* DMA1_CH3_Handler	*/
	.long	Default_Handler /* DMA1_CH4_Handler	*/
	.long	Default_Handler /* DMA1_CH5_Handler	*/
	.long	Default_Handler /* DMA1_CH6_Handler	*/
	.long	Default_Handler /* DMA1_CH7_Handler	*/
	.long	Default_Handler /* ADC1_2_Handler	*/
	.long	Default_Handler /* CAN_TX_Handler	*/
	.long	Default_Handler /* CAN_RX0_Handler	*/
	.long	Default_Handler /* CAN_RX1_Handler	*/
	.long	Default_Handler /* CAN_SCE_Handler	*/
	.long	Default_Handler /* EXTI9_5_Handler	*/
	.long	Default_Handler /* TIM1_BRK_Handler	*/
	.long	Default_Handler /* TIM1_UP_Handler	*/
	.long	Default_Handler /* TIM1_TRG_COM_Handler	*/
	.long	Default_Handler /* TIM1_CC_Handler	*/
	.long	TIM2_Handler	/* TIM2_Handler		*/
	.long	TIM3_Handler	/* TIM3_Handler		*/
	.long	TIM4_Handler	/* TIM4_Handler		*/
	.long	Default_Handler /* I2C1_EV_Handler	*/
	.long	Default_Handler /* I2C1_ER_Handler	*/
	.long	Default_Handler /* I2C2_EV_Handler	*/
	.long	Default_Handler /* I2C2_ER_Handler	*/
	.long	Default_Handler /* SPI1_Handler		*/
	.long	Default_Handler /* SPI2_Handler		*/
	.long	USART1_Handler  /* USART1_Handler	*/
	.long	USART2_Handler  /* USART2_Handler	*/
	.long	USART3_Handler  /* USART3_Handler	*/
	.long	Default_Handler /* EXTI15_10_Handler	*/
	.long	Default_Handler /* RTC_ALARM_Handler	*/
	.long	Default_Handler /* USB_WAKEUP_Handler	*/
	.long	Default_Handler /* TIM8_BRK_Handler	*/
	.long	Default_Handler /* TIM8_UP_Handler	*/
	.long	Default_Handler /* TIM8_TRG_COM_Handler	*/
	.long	Default_Handler /* TIM8_CC_Handler	*/
	.long	Default_Handler /* ADC3_Handler		*/
	.long	Default_Handler /* FSMC_Handler		*/
	.long	Default_Handler /* SDIO_Handler		*/
	.long	TIM5_Handler	/* TIM5_Handler		*/
	.long	Default_Handler /* SPI3_Handler		*/
	.long	USART4_Handler  /* UART4_Handler	*/
	.long	USART5_Handler  /* UART5_Handler	*/
	.long	TIM6_Handler	/* TIM6_Handler		*/
	.long	TIM7_Handler	/* TIM7_Handler		*/
	.long	Default_Handler /* DMA2_CH1_Handler	*/
	.long	Default_Handler /* DMA2_CH2_Handler	*/
	.long	Default_Handler /* DMA2_CH3_Handler	*/
	.long	Default_Handler /* DMA2_CH4_CH5_Handler */

	.balignl 512,0xdeadbeef	/* aligned multiple of 512 B, fill long word 0xdeadbeef */
	/*.balignw 512,0xdead*/ /* aligned multiple of 512 B, fill word 0xdead */
	/*.align	9*/ 	/* aligned 2^9 B, fill 0 */

	.align 2
	.thumb_func
	.type Reset_Handler, %function
Reset_Handler:
	/* copy data to SRAM from FLASH */
	ldr	r1, =__rodata_end__
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__
cpdata_s:
	cmp	r2,r3
	beq	cpdata_e
	ldmia	r1!,{r0}
	stmia	r2!,{r0}
	b	cpdata_s
cpdata_e:
	/* clear BSS block */
 	mov	r0, #0x0
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
clrbss_s:
	cmp	r1, r2
	beq	clrbss_e
	stmia	r1!,{r0}
	b	clrbss_s
clrbss_e:
	mov	r1,r0
	mov	r2,r0
	mov	r3,r0
	mov	r4,r0
	mov	r5,r0
	mov	r6,r0
	mov	r7,r0
	mov	r8,r0
	mov	r9,r0
	mov	r10,r0
	mov	r11,r0
	mov	r12,r0

	/* init sp,8 bytes aligned */
	ldr	sp, =((CONFIG_STACK_INIT_SP) & ~0x0007)

	/* init AHB,APB,System clock */
	bl	board_init

	/* Jump into main */
	bl	main

	/* jump into dead loop after returned from main */
9:	nop
	b	9b

	.align 2
	.thumb_func
	.type handle_deadloop, %function
handle_deadloop:
	nop
	b	handle_deadloop

	.macro def_irq_handler handler_name
	.weak	\handler_name
	.set	\handler_name, handle_deadloop
	.endm

	def_irq_handler NMI_Handler
	def_irq_handler HardFault_Handler
	def_irq_handler MemManage_Handler
	def_irq_handler BusFault_Handler
	def_irq_handler UsageFault_Handler
	def_irq_handler SVC_Handler
	def_irq_handler DebugMon_Handler
	def_irq_handler PendSV_Handler
	def_irq_handler SysTick_Handler
	def_irq_handler USART1_Handler
	def_irq_handler USART2_Handler
	def_irq_handler USART3_Handler
	def_irq_handler USART4_Handler
	def_irq_handler USART5_Handler
	def_irq_handler TIM2_Handler
	def_irq_handler TIM3_Handler
	def_irq_handler TIM4_Handler
	def_irq_handler TIM5_Handler
	def_irq_handler TIM6_Handler
	def_irq_handler TIM7_Handler
	def_irq_handler Default_Handler
