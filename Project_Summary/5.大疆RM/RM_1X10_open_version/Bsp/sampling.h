#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "adc.h"

				
typedef struct 
{
   uint8_t value[ADC_NUM];
} Hc4051AdcValue;


void ReadAdcValue(Hc4051AdcValue *adc_raw_value);
#endif
