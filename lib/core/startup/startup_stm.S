/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
 *
 Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#define CONFIG_STACK_INIT_SP 0x20005000

	.syntax	unified
	.arch	armv7-m

.globl	_start
_start:
	.long	CONFIG_STACK_INIT_SP  /* Top of Stack */
	.long	Reset_Handler         /* Reset Handler */
	.long	NMI_Handler           /* NMI Handler */
	.long	HardFault_Handler     /* Hard Fault Handler */
	.long	MemManage_Handler     /* MPU Fault Handler */
	.long	BusFault_Handler      /* Bus Fault Handler */
	.long	UsageFault_Handler    /* Usage Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVC_Handler           /* SVCall Handler */
	.long	DebugMon_Handler      /* Debug Monitor Handler */
	.long	0                     /* Reserved */
	.long	PendSV_Handler        /* PendSV Handler */
	.long	SysTick_Handler       /* SysTick Handler */
	.long	Default_Handler

	.align	8
/*.globl	Reset_Handler*/
Reset_Handler:
	ldr	r1, =__text_end__
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__
cpdata_start:
	cmp	r2,r3
/*	ldrlo	r0, [r1], #4
	strlo	r0, [r2], #4
	blo	cpdata_start
*/
	mov r0, #0x00000000
	mov r3,r0
	mov r4,r0
	mov r5,r0
	mov r6,r0
	mov r7,r0
	mov r8,r0
	mov r9,r0
	mov r10,r0
	mov r11,r0
	mov r12,r0

	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
clr_bss:
	cmp	r1, r2
/*	strlo	r0, [r1], #4
	blo	clr_bss
*/
	ldr	sp, =((CONFIG_STACK_INIT_SP) & ~0x0007)

	bl	SystemInit

	bl	main

.macro irq_save_regs

.endm

.macro irq_restore_regs

.endm

	.align	2
.global do_interrupts
Default_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
NMI_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
HardFault_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
MemManage_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
BusFault_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
UsageFault_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
SVC_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
DebugMon_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
PendSV_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
SysTick_Handler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs

	.align	2
DEF_IRQHandler:
	irq_save_regs
	bl	do_interrupts
	irq_restore_regs
