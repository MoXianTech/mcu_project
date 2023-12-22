#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (26 * 220) //9 ns * 120


void Divider_Resistance_Selection(uint8_t data);
void Hc4051Delay(uint32_t num)
{
    while(num --);
}

/*
功能：Init
输入：none
*/ 
void Hc4051IoInit(void)
{

		rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_GPIOB);
		rcu_periph_clock_enable(RCU_GPIOC);
		rcu_periph_clock_enable(RCU_GPIOD);  
  
		rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15);	  
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_ALL);
		gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_ALL);
		gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);			
		GREEN_LED(0);
}

/*
功能：AY SCAN
输入：data 0 -> 7
*/   
void Choose_AY_Channel(uint8_t data)
{                                                                                            
    C_A( data & 0x01);                                              
    C_B((data & 0x02) >> 1);
		C_C((data & 0x04) >> 2);
}


/*
功能：AX Scan
输入：data 
*/
void Choose_AX_Channel(uint8_t index)
{
			
/*32x64 AX_Channel Selection*/		
		uint32_t GPIO_Activate_Array[32] = {	0x00000001, 0x00000002, 0x00000004, 0x00000008, 
																					0x00000010, 0x00000020, 0x00000040, 0x00000080,
																					0x00000100, 0x00000200, 0x00000400, 0x00000800,
																					0x00001000, 0x00002000, 0x00004000, 0x00008000,
																					0x00010000, 0x00020000, 0x00040000, 0x00080000,
																					0x00100000, 0x00200000, 0x00400000, 0x00800000,
																					0x01000000, 0x02000000, 0x04000000, 0x08000000,			
																					0x10000000, 0x20000000, 0x40000000, 0x80000000 };
		AX25( GPIO_Activate_Array[index] & 0x00000001);
		AX24((GPIO_Activate_Array[index] & 0x00000002) >> 1);
		AX23((GPIO_Activate_Array[index] & 0x00000004) >> 2);
		AX22((GPIO_Activate_Array[index] & 0x00000008) >> 3);
		AX15((GPIO_Activate_Array[index] & 0x00000010) >> 4);		
		AX14((GPIO_Activate_Array[index] & 0x00000020) >> 5);	
		AX13((GPIO_Activate_Array[index] & 0x00000040) >> 6);
		AX12((GPIO_Activate_Array[index] & 0x00000080) >> 7);	
		AX26((GPIO_Activate_Array[index] & 0x00000100) >> 8);				
		AX27((GPIO_Activate_Array[index] & 0x00000200) >> 9);
		AX28((GPIO_Activate_Array[index] & 0x00000400) >> 10);																				
		AX31((GPIO_Activate_Array[index] & 0x00000800) >> 11);			
		AX3 ((GPIO_Activate_Array[index] & 0x00001000) >> 12);
		AX4 ((GPIO_Activate_Array[index] & 0x00002000) >> 13);			
		AX5 ((GPIO_Activate_Array[index] & 0x00004000) >> 14);	
		AX6 ((GPIO_Activate_Array[index] & 0x00008000) >> 15);	
		AX7 ((GPIO_Activate_Array[index] & 0x00010000) >> 16);	
		AX8 ((GPIO_Activate_Array[index] & 0x00020000) >> 17);
		AX9 ((GPIO_Activate_Array[index] & 0x00040000) >> 18);
		AX11((GPIO_Activate_Array[index] & 0x00080000) >> 19);
		AX10((GPIO_Activate_Array[index] & 0x00100000) >> 20);	
		AX2 ((GPIO_Activate_Array[index] & 0x00200000) >> 21);
		AX1 ((GPIO_Activate_Array[index] & 0x00400000) >> 22);
		AX0 ((GPIO_Activate_Array[index] & 0x00800000) >> 23);	
		AX21((GPIO_Activate_Array[index] & 0x01000000) >> 24);
		AX20((GPIO_Activate_Array[index] & 0x02000000) >> 25);
		AX30((GPIO_Activate_Array[index] & 0x04000000) >> 26);	
		AX29((GPIO_Activate_Array[index] & 0x08000000) >> 27);		
		AX16((GPIO_Activate_Array[index] & 0x10000000) >> 28);
		AX17((GPIO_Activate_Array[index] & 0x20000000) >> 29);
		AX18((GPIO_Activate_Array[index] & 0x40000000) >> 30);
		AX19((GPIO_Activate_Array[index] & 0x80000000) >> 31);
}


/*
功能：行列扫描读取数据
输入：无
返回：返回32*64的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{	
			/*32x64 Data_SCAN*/
			uint8_t adc_rank[8] = {2,1,0,3,5,7,6,4};  
			for(int8_t AY = 0; AY < 8; ++AY)
			{
					Choose_AY_Channel(adc_rank[AY]);
					for(int8_t AX = 0; AX < AX_NUM; ++AX)
					{							
						Choose_AX_Channel(AX);
						Hc4051Delay(ADC_DELAY_TIME);
						/*32x64 ADC_Value*/	
						adc_raw_value->value[AX][AY]      = GetAdcValue(3);
						adc_raw_value->value[AX][AY +  8] = GetAdcValue(2);				
						adc_raw_value->value[AX][AY + 16] = GetAdcValue(1);
						adc_raw_value->value[AX][AY + 24] = GetAdcValue(0);		
		
				}
			}		
}


void ProcessADCvalue(Hc4051AdcValue *adc_raw_value)
{
		uint16_t Vref = 4096;
		uint16_t flag_sum = 1;
		uint16_t display_threshold_value = 0;
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

										adc_raw_value->voltage[AX][AY] 
										=	 (adc_raw_value->value[AX][AY] * (uint32_t)255 
											/ (Vref - (adc_raw_value->sumAX[AY] - adc_raw_value->value[AX][AY])));	
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




