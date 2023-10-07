#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (15 * 220) //9 ns * 120


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

		rcu_periph_clock_enable(RCU_GPIOB);
		rcu_periph_clock_enable(RCU_GPIOC);
		rcu_periph_clock_enable(RCU_GPIOD);
		rcu_periph_clock_enable(RCU_GPIOE);
    
		rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL );
		gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL );
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 );
		gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 );
			
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
			
/*32x64 AX_Channel Selection*/		
		uint16_t GPIOD_Activate_Array[16] = {	1, 2,  4,  8, 16, 32, 64, 128, 32768, 16384, 8192, 4096, 2048, 1024, 512, 256};//256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 1, 2, 4, 8, 16, 32, 64,128}; 
		uint16_t GPIOE_Activate_Array[16] = { 1, 2,  4,  8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
		uint16_t GPIOD_write, GPIOE_write;
		
		if(data < 16 )
		{
			gpio_port_write(GPIOE, 0 );	
			GPIOD_write = GPIOD_Activate_Array[ data ];
			gpio_port_write(GPIOD, GPIOD_write);
		} 
		else  
		{
		gpio_port_write(GPIOD, 0 );
		GPIOE_write = GPIOE_Activate_Array[ data - 16 ];	
		gpio_port_write(GPIOE, GPIOE_write);
		}			
}


/*
功能：行列扫描读取数据
输入：无
返回：返回32*64的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{	
			/*32x64 Data_SCAN*/
			for(int8_t AY = 0; AY < 8; ++AY)
			{
					Choose_AY_Channel(AY);
					for(int8_t AX = 0; AX < AX_NUM; ++AX)
					{							
						Choose_AX_Channel(AX);
						Hc4051Delay(ADC_DELAY_TIME);
						/*32x64 ADC_Value*/	
						adc_raw_value->value[AX][AY]      = GetAdcValue(6); //6 7 4 5 3 2 1 0
						adc_raw_value->value[AX][AY +  8] = GetAdcValue(7);				
						adc_raw_value->value[AX][AY + 16] = GetAdcValue(4);
						adc_raw_value->value[AX][AY + 24] = GetAdcValue(5);			
						adc_raw_value->value[AX][AY + 32] = GetAdcValue(3);
						adc_raw_value->value[AX][AY + 40] = GetAdcValue(2);	
						adc_raw_value->value[AX][AY + 48] = GetAdcValue(1);
						adc_raw_value->value[AX][AY + 56] = GetAdcValue(0);			
				}
			}		
}


void ProcessADCvalue(Hc4051AdcValue *adc_raw_value)
{
		uint16_t Vref = 4096;
		uint16_t flag_sum = 1;
		uint16_t display_threshold_value = 50;
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
										= (adc_raw_value->value[AX][AY] * Vref) / (Vref - (adc_raw_value->sumAX[AY] - adc_raw_value->value[AX][AY]));
									adc_raw_value->d_voltage[AX][AY] 
										= 1000000 / (Vref - adc_raw_value->voltage[AX][AY]) - 244;				
								}
								else
								{
									adc_raw_value->voltage[AX][AY] = 0;
									adc_raw_value->d_voltage[AX][AY] = 0;
								}
							}
				} 	
			
				memset(adc_raw_value->sumAX, 0 , sizeof(adc_raw_value->sumAX));	
				flag_sum = 1;
		}
}

void display_remaping(Hc4051AdcValue *adc_raw_value)
{
	for(uint8_t ax = 0; ax < 31; ++ax)
	{
		for(uint8_t ay = 0; ay < 51; ++ay)
		{
			adc_raw_value->d_voltage_display[ax][50 - ay] = adc_raw_value->d_voltage[ax][ay + 13];	
		}
	}
	
	
}