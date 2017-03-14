/*
 * APSR: [31]-N, [30]-Z, [29]-C, [28]-V, [27]-Q
 * IPSR: [8:0] - IRQ number
 * EPSR: [24]-T
 */
	.syntax	unified
	.arch armv7-m
	.cpu cortex-m3
	.thumb

.global __get_XPSR
__get_XPSR:
	mrs	r0,psr
	isb
	mov	pc,lr

.global __get_APSR
__get_APSR:
	mrs	r0,apsr
	isb
	mov	pc,lr

.global __get_IPSR
__get_IPSR:
	mrs	r0,ipsr
	isb
	mov	pc,lr

.global __get_EPSR
__get_EPSR:
	mrs	r0,epsr
	isb
	mov	pc,lr

.global __set_APSR
__set_APSR:
	msr	apsr_nzcvq,r0
	isb
	mov	pc,lr

.global __get_PRIMASK
__get_PRIMASK:
	mrs	r0,primask
	isb
	mov	pc,lr

.global __set_PRIMASK
__set_PRIMASK:
	msr	primask,r0
	isb
	mov	pc,lr

.global __get_FAULTMASK
__get_FAULTMASK:
	mrs	r0,faultmask
	isb
	mov	pc,lr

.global __set_FAULTMASK
__set_FAULTMASK:
	msr	faultmask,r0
	isb
	mov	pc,lr

.global __get_BASEPRI
__get_BASEPRI:
	mrs	r0,basepri
	isb
	mov	pc,lr

.global __set_BASEPRI
__set_BASEPRI:
	msr	basepri,r0
	isb
	mov	pc,lr

.global __disable_irq
__disable_irq:
	mrs	r0,primask
	orr	r0,r0,#0x1
	msr	primask,r0
	isb
	mov	pc,lr

.global __enable_irq
__enable_irq:
	mrs	r0,primask
	bic	r0,r0,#0x1
	msr	primask,r0
	isb
	mov	pc,lr

.global __get_CONTROL
__get_CONTROL:
	mrs	r0,control
	isb
	mov	pc,lr

.global	__set_CONTROL
__set_CONTROL:
	msr	control,r0
	isb
	mov	pc,lr
