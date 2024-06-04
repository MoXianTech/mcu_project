#ifndef __LED_h
#define __LED_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "stdio.h"

/*LED*/
#define GREEN_LED(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_5)											
#define RED_LED(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_4)	
				
//Co_Powered_PINS
#define CO_Powered_Pin(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)	

void led_gpio_init(void);
void led_spark(void);
void SW_IoInit(void);

#endif

