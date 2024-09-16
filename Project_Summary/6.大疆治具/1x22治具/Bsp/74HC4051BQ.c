#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (5 * 120) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}

void Hc4051IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
		
	  gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8| GPIO_PIN_11| GPIO_PIN_12);
															
		GREEN_LED(0);
}



/*
功能：选择行导通
输入：第几行
*/
void AY_Selection(uint8_t data)
{
    CA( data & 0x01);
    CB((data & 0x02) >> 1);
}
/*


/*
功能：行列扫描读取数据
输入：无
返回：返回16*16的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value, uint8_t selector_channel_index)
{
		
		AY_Selection(selector_channel_index);	
		Hc4051Delay(ADC_DELAY_TIME);

		adc_raw_value->value[selector_channel_index]      =  255 * (uint32_t)GetAdcValue(5) / 4096;
		adc_raw_value->value[selector_channel_index +  4] =  255 * (uint32_t)GetAdcValue(4) / 4096;
		adc_raw_value->value[selector_channel_index +  8] =  255 * (uint32_t)GetAdcValue(3) / 4096;
		adc_raw_value->value[selector_channel_index + 12] =  255 * (uint32_t)GetAdcValue(2) / 4096;
		adc_raw_value->value[selector_channel_index + 16] =  255 * (uint32_t)GetAdcValue(1) / 4096;
		adc_raw_value->value[selector_channel_index + 20] =  255 * (uint32_t)GetAdcValue(0) / 4096;
}
	
		

void display_reampping(Hc4051AdcValue *adc_raw_value)
{
//		uint8_t AY_display[20] = {5,7,6,4, 9,11,10,8, 13,  20,22,23,21, 16,18,19,17, 12,14,15};
		uint8_t AY_display[20] = {13, 8,10,11,9, 4,6,7,5,  15,14,12, 17,19,18,16, 21,23,22,20};
		adc_raw_value->voltage_display[10] = adc_raw_value->value[1];
		adc_raw_value->voltage_display[9] = adc_raw_value->value[0];
		for(uint8_t AY = 0; AY < 9; ++AY)
		{
			adc_raw_value->voltage_display[AY] = adc_raw_value->value[AY_display[AY]];
		} 
	
		for(uint8_t AY = 9; AY < 20; ++AY)
		{
			adc_raw_value->voltage_display[AY + 2] = adc_raw_value->value[AY_display[AY]];
		} 
	
}


