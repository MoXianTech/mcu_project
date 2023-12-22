#ifndef __LED_H
#define __LED_H

#include "gd32e10x.h"

#define GREEN_LED(a)			if(a)		\
														gpio_bit_set(GPIOA, GPIO_PIN_15);	\
													else		\
														gpio_bit_reset(GPIOA, GPIO_PIN_15)

void led_init(void);

#endif

