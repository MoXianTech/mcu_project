#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (7 * 220) //9 ns * 120


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
		rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15);
		gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL );
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL );
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
void Choose_AX_Channel(uint16_t data)
{
		uint32_t AX_select_rank[32] = {0x00000001, 0x00000002, 0x00000004, 0x00000008, 
																	 0x00000010, 0x00000020, 0x00000040, 0x00000080,
																	 0x00000100, 0x00000200, 0x00000400, 0x00000800,
																	 0x00001000, 0x00002000, 0x00004000, 0x00008000,
																	 0x00010000, 0x00020000, 0x00040000, 0x00080000, 
																	 0x00100000, 0x00200000, 0x00400000, 0x00800000,
																	 0x01000000, 0x02000000, 0x04000000, 0x08000000, 
																	 0x10000000, 0x20000000, 0x40000000, 0x80000000};	
		AX31(AX_select_rank[data]  & AX_select_rank[0]);
		AX30((AX_select_rank[data] & AX_select_rank[1]) >> 1);
		AX29((AX_select_rank[data] & AX_select_rank[2]) >> 2);		
		AX25((AX_select_rank[data] & AX_select_rank[3]) >> 3);	
		AX24((AX_select_rank[data] & AX_select_rank[4]) >> 4);
		AX23((AX_select_rank[data] & AX_select_rank[5]) >> 5);
		AX22((AX_select_rank[data] & AX_select_rank[6]) >> 6);
		AX14((AX_select_rank[data] & AX_select_rank[7]) >> 7);
		AX12((AX_select_rank[data] & AX_select_rank[8]) >> 8);
		AX15((AX_select_rank[data] & AX_select_rank[9]) >> 9);
		AX13((AX_select_rank[data] & AX_select_rank[10])>> 10);
		AX11((AX_select_rank[data] & AX_select_rank[11])>> 11);
		AX10((AX_select_rank[data] & AX_select_rank[12])>> 12);
		AX1((AX_select_rank[data]  & AX_select_rank[13])>> 13);
		AX21((AX_select_rank[data] & AX_select_rank[14])>> 14);
		AX2((AX_select_rank[data]  & AX_select_rank[15])>> 15);
																	 
		AX9((AX_select_rank[data]  & AX_select_rank[16])>> 16);
		AX6((AX_select_rank[data] & AX_select_rank[17])>> 17);
		AX4((AX_select_rank[data] & AX_select_rank[18])>> 18);
		AX8((AX_select_rank[data] & AX_select_rank[19])>> 19);
		AX7((AX_select_rank[data] & AX_select_rank[20])>> 20);
		AX5((AX_select_rank[data]  & AX_select_rank[21])>> 21);
		AX16((AX_select_rank[data] & AX_select_rank[22])>> 22);
		AX17((AX_select_rank[data] & AX_select_rank[23])>> 23);
		AX18((AX_select_rank[data] & AX_select_rank[24])>> 24);
		AX19((AX_select_rank[data] & AX_select_rank[25])>> 25);
		AX3((AX_select_rank[data]  & AX_select_rank[26])>> 26);
		AX28((AX_select_rank[data]  & AX_select_rank[27])>> 27);
		AX27((AX_select_rank[data]  & AX_select_rank[28])>> 28);
		AX26((AX_select_rank[data]  & AX_select_rank[29])>> 29);
		AX20((AX_select_rank[data]  & AX_select_rank[30])>> 30);
		AX0((AX_select_rank[data]  & AX_select_rank[31])>> 31);		

}


/*
功能：行列扫描读取数据
输入：无
返回：返回32*64的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value, uint8_t count)
	
{	
			uint8_t adc_rank[8] = {3,0,1,2,4,6,7,5}; 
			/*32x64 Data_SCAN*/
			for(uint8_t AY = 0; AY < 8; ++AY)
			{
				Choose_AY_Channel(adc_rank[AY]);
				Hc4051Delay(ADC_DELAY_TIME);
				/*32x64 ADC_Value*/	
				adc_raw_value->value[count][AY]      = GetAdcValue(0);
				adc_raw_value->value[count][AY +  8] = GetAdcValue(1);				
				adc_raw_value->value[count][AY + 16] = GetAdcValue(2);
				adc_raw_value->value[count][AY + 24] = GetAdcValue(3);			
				adc_raw_value->value[count][AY + 32] = GetAdcValue(4);
				adc_raw_value->value[count][AY + 40] = GetAdcValue(5);	
				adc_raw_value->value[count][AY + 48] = GetAdcValue(6);
				adc_raw_value->value[count][AY + 56] = GetAdcValue(7);			
			}
}


void ProcessADCvalue(Hc4051AdcValue *adc_raw_value)
{
		uint16_t Vref = 4095;
		uint16_t flag_sum = 1;
		uint16_t display_threshold_value = 5;
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
										=	 ((uint32_t)adc_raw_value->value[AX][AY] * 255 
											/ (Vref - (adc_raw_value->sumAX[AY] - adc_raw_value->value[AX][AY])));

								}
								else
								{
									adc_raw_value->voltage[AX][AY] = 0;
								}
							}
				} 	
//				printf("%u \n", adc_raw_value->d_voltage[0][0]);				
				memset(adc_raw_value->sumAX, 0 , sizeof(adc_raw_value->sumAX));	
				flag_sum = 1;
		}
}

void display_remapping(Hc4051AdcValue *adc_raw_value)
{

}

