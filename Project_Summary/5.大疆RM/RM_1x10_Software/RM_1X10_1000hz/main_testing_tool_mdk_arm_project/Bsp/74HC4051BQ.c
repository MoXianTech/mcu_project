#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (10) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}

void Hc4051IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	

	    CO_Powered_Pin(1); // 1 - Sensor_Power On, 0 - OFF
}


/*
功能：GET ADC_VALUE
输入：无
返回：
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{

//			Hc4051Delay(ADC_DELAY_TIME);
     for(int i = 0; i < 10; ++i)
		 {
		 	adc_raw_value->value[i] = 255 * (uint32_t)GetAdcValue(i) / 4095;
			if(adc_raw_value->value[i] > 255) adc_raw_value->value[i] = 255;
		 }			 
		

}
    


