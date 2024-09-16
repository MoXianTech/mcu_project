#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"


//Co_Powered_PINS
#define CO_Powered_Pin(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)	
					


				
typedef struct 
{
   uint8_t value[10];
} Hc4051AdcValue;

uint8_t GetSwStatus(void);
void Hc4051IoInit(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value);
void ChooseEnlarge(uint8_t data);
#endif
