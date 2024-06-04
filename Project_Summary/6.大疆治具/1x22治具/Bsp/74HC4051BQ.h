#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"

#define ADC_NUM 24

#define GREEN_LED(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_8)						
						

// ÐÐÑ¡	R1~R16			
#define CA(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_11)
#define CB(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_12)


			
typedef struct 
{
   uint8_t value[ADC_NUM];
	 uint8_t voltage_display[ADC_NUM - 2];
} Hc4051AdcValue;

uint8_t GetSwStatus(void);
void Hc4051IoInit(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value, uint8_t selector_channel_index);
void display_reampping(Hc4051AdcValue *adc_raw_value);
void ChooseEnlarge(uint8_t data);
#endif
