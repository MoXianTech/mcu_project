#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (26 * 220) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}

void Hc4051IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
	
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_7  |
																													GPIO_PIN_8 | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | 
																													GPIO_PIN_12| GPIO_PIN_13 | GPIO_PIN_14 );
	
			GREEN_LED(0);
//			ChooseEnlarge(0); 
}




/*
功能：选择列导通
输入：第几列 
*/
void Co_Powered_PINS(uint32_t data)
{
	uint32_t Column_Choose_Array[10]={1,2,4,8,16,32,64,128,256,512};
	 
	 R1(  Column_Choose_Array[data] &   1);
	 R2( (Column_Choose_Array[data] &   2) >> 1);
	 R3( (Column_Choose_Array[data] &   4) >> 2);
	 R4( (Column_Choose_Array[data] &   8) >> 3);
	 R5( (Column_Choose_Array[data] &  16) >> 4);
	 R6( (Column_Choose_Array[data] &  32) >> 5);
	 R7( (Column_Choose_Array[data] &  64) >> 6);
	 R8( (Column_Choose_Array[data] & 128) >> 7);
	 R9( (Column_Choose_Array[data] & 256) >> 8);
	 R10((Column_Choose_Array[data] & 512) >> 9);

}

/*
功能：行列扫描读取数据
输入：无
返回：
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{

			for(uint16_t R = 0; R < 10; ++R)
			{
				Co_Powered_PINS(R);

				Hc4051Delay(ADC_DELAY_TIME);
						
//				adc_raw_value->value[R][0] = GetAdcValue(0) / 2; 
//				adc_raw_value->value[R][1] = GetAdcValue(3) / 2;
//				adc_raw_value->value[R][2] = GetAdcValue(2) / 2;
//				adc_raw_value->value[R][3] = GetAdcValue(4) / 2;
//				adc_raw_value->value[R][4] = GetAdcValue(1) / 2;
				adc_raw_value->value[R][0] = 255 * GetAdcValue(0) / 4096; 
				adc_raw_value->value[R][1] = 255 * GetAdcValue(3) / 4096;
				adc_raw_value->value[R][2] = 255 * GetAdcValue(2) / 4096;
				adc_raw_value->value[R][3] = 255 * GetAdcValue(4) / 4096;
				adc_raw_value->value[R][4] = 255 * GetAdcValue(1) / 4096;
		
			}
	}

void ProcessADCvalue(Hc4051AdcValue *adc_raw_value)
{
		uint16_t Vref = 4096;
		uint16_t flag_sum = 1;
		uint16_t display_threshold_value = 40;
		while(flag_sum)
		{					
				for(uint8_t AY = 0; AY < AY_NUM; ++AY)
				{
						for(uint8_t AX = 0; AX < AX_NUM; ++AX)
						{
								adc_raw_value->sumAX[AY] += adc_raw_value->value[AX][AY];
						}
				}	
				flag_sum = 0;						
		}
		
		while(flag_sum == 0)
		{
				
				for(uint8_t AY = 0; AY < AY_NUM; ++AY)
				{			
							for(uint8_t AX = 0; AX < AX_NUM; ++AX)
							{	
								if(adc_raw_value->value[AX][AY] > display_threshold_value)
								{									
									adc_raw_value->voltage[AX][AY] = (adc_raw_value->value[AX][AY] * 255) / (Vref - (adc_raw_value->sumAX[AY] - adc_raw_value->value[AX][AY]));														
								}
								else
								{
									adc_raw_value->voltage[AX][AY] = 0;
								}
							}
				} 		
				memset(adc_raw_value->sumAX, 0 , sizeof(adc_raw_value->sumAX));	
				flag_sum = 1;
		}
}
	
