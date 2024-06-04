#include "sampling.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;


void Hc4051Delay(uint32_t num)
{
    while(num --);
}

void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{
     for(int i = 0; i < ADC_NUM; ++i)
		 {
				adc_raw_value->value[i] = 255 * (uint32_t)GetAdcValue(i) / 4095;
				if(adc_raw_value->value[i] >= 255) 
				{
					adc_raw_value->value[i] = 255;
				}
		 }			 
}
    


