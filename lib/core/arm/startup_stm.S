#define CONFIG_STACK_INIT_SP 0x20005000

	.syntax	unified
	.arch	armv7-m

.globl	_start
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
	.long	Default_Handler /* TIM2_Handler		*/
	.long	Default_Handler /* TIM3_Handler		*/
	.long	Default_Handler /* TIM4_Handler		*/
	.long	Default_Handler /* I2C1_EV_Handler	*/
	.long	Default_Handler /* I2C1_ER_Handler	*/
	.long	Default_Handler /* I2C2_EV_Handler	*/
	.long	Default_Handler /* I2C2_ER_Handler	*/
	.long	Default_Handler /* SPI1_Handler		*/
	.long	Default_Handler /* SPI2_Handler		*/
	.long	Default_Handler /* USART1_Handler	*/
	.long	Default_Handler /* USART2_Handler	*/
	.long	Default_Handler /* USART3_Handler	*/
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
	.long	Default_Handler /* TIM5_Handler		*/
	.long	Default_Handler /* SPI3_Handler		*/
	.long	Default_Handler /* UART4_Handler	*/
	.long	Default_Handler /* UART5_Handler	*/
	.long	Default_Handler /* TIM6_Handler		*/
	.long	Default_Handler /* TIM7_Handler		*/
	.long	Default_Handler /* DMA2_CH1_Handler	*/
	.long	Default_Handler /* DMA2_CH2_Handler	*/
	.long	Default_Handler /* DMA2_CH3_Handler	*/
	.long	Default_Handler /* DMA2_CH4_CH5_Handler */

	.align	9

Reset_Handler:
/*
 * Copy data to SRAM from FLASH
 */
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
	mov	r0,#0x00000000
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
/*
 * Clear BSS block
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
clrbss_s:
	cmp	r1, r2
	beq	clrbss_e
	stmia	r1!,{r0}
	b clrbss_s

clrbss_e:
/*
 * Config stack pointer
 */
	ldr	sp, =((CONFIG_STACK_INIT_SP) & ~0x0007)
/*
 * Set CPU to SVC mode
 */
/*
	mrs	r0,apsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
 	bic	r0,r0,#0x80
	msr	apsr_nzcvq,r0
*/
/*
 * Config AHB,APB,System clock
 */
	mov	r0,#0
	mov	r1,r0
	mov	r2,r0
	bl	board_init
/*
 * Jump into main
 */
	bl	main
/*
 * After returned from main, stop here
 */
9:	nop
	b	9b

#if 0
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52
#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

	.macro irq_save_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0-r12}
	str	lr, [sp,#S_LR]
	mrs	r0, cpsr
	str	r0, [sp,#S_PSR]
	.endm

	.macro irq_restore_regs
	ldr	r0, [sp,#S_PSR]
	msr	cpsr,r0
	ldr	lr, [sp,#S_LR]
	ldmia	sp, [r0-r12]
	add	sp, sp, #S_FRAME_SIZE
	.endm
#else
	.macro irq_save_regs
	/*
	 * ARM CM3 can save IRQ frame into memory automatically.
	 */
	.endm
	.macro irq_restore_regs
	/*
	 * Nothing to do
	 */
	.endm
#endif

.global do_nmi
.global do_hardfault
.global do_memmanage
.global do_busfault
.global do_usagefault
.global do_svc
.global do_debugmon
.global do_pendsv
.global do_systick
.global do_interrupts

	.align	2
NMI_Handler:
	irq_save_regs
	bl	do_nmi
	irq_restore_regs

	.align	2
HardFault_Handler:
	irq_save_regs
	bl	do_hardfault
	irq_restore_regs

	.align	2
MemManage_Handler:
	irq_save_regs
	bl	do_memmanage
	irq_restore_regs

	.align	2
BusFault_Handler:
	irq_save_regs
	bl	do_busfault
	irq_restore_regs

	.align	2
UsageFault_Handler:
	irq_save_regs
	bl	do_usagefault
	irq_restore_regs

	.align	2
SVC_Handler:
	irq_save_regs
	bl	do_svc
	irq_restore_regs

	.align	2
DebugMon_Handler:
	irq_save_regs
	bl	do_debugmon
	irq_restore_regs

	.align	2
PendSV_Handler:
	irq_save_regs
	bl	do_pendsv
	irq_restore_regs

	.align	2
SysTick_Handler:
	irq_save_regs
	bl	do_systick
	irq_restore_regs

	.align	2
Default_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs
