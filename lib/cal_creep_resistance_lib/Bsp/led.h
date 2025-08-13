#ifndef _LED_H
#define _LED_H

#include "gd32f4xx.h"

#define GREEN_LED(a)	if (a)	\
    gpio_bit_set(GPIOC, GPIO_PIN_6);\
    else		\
    gpio_bit_reset(GPIOC, GPIO_PIN_6)
		
void led_init(void);

#endif
