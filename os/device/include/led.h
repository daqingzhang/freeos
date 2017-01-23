#ifndef __LED_H__
#define __LED_H__

#define LED1	(1<<0)
#define LED2	(1<<1)
#define LED3	(1<<2)

void led_init(unsigned int led);
void led_on(unsigned int led);
void led_off(unsigned int led);
void led_flash(unsigned int led, unsigned int on,unsigned int off);

#endif
