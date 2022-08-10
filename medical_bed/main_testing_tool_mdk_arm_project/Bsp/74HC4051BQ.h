#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"

#define GREEN_LED(a)	if (a)	\
							gpio_bit_set(GPIOA, GPIO_PIN_12);\
						else		\
							gpio_bit_reset(GPIOA, GPIO_PIN_12)						
		

#define YC_N_0(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_15)				

#define YC_N_1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_14);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_14)				

#define YC_N_2(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_13);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_13)				

#define YC_N_3(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_12)				



#define YC_A(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)				

#define YC_B(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_1)				

#define YC_C(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_0);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_0)				

	
#define XC_A(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_3);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_3)				
	
#define XC_B(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_4)				
	
#define XC_C(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_5)				
	
typedef struct 
{
    uint16_t value[16][16];
} Hc4051AdcValue;

void Hc4051IoInit(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value);
void select_x_control(uint8_t x_value);
void select_y_control(uint8_t y_value);

#endif
