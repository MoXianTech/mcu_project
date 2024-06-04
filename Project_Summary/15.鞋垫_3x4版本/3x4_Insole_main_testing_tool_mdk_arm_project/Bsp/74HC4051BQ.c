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
    rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
		
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, 
															GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3|
														  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8|
														  GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14|
															GPIO_PIN_15);
	  gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,
															GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
														  GPIO_PIN_8| GPIO_PIN_11| GPIO_PIN_12 | GPIO_PIN_15);
															
		GREEN_LED(0);
		ChooseEnlarge(2); 
}



/*
功能：选择分压电阻
输入：0~3对应的D电阻为：4.7K、100R、200R、1K
*/
void ChooseEnlarge(uint8_t data)
{
    MA( data & 0x01);
    MB((data & 0x02) >> 1);
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

功能：选择列导通
输入：第几列 
*/
void AX_Selection(uint16_t data)
{
		uint32_t AX_Choose_Array[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
		
		AX16(AX_Choose_Array[data]  & 1);
		AX15((AX_Choose_Array[data] & 2)     >> 1);
		AX14((AX_Choose_Array[data] & 4)     >> 2);
		AX13((AX_Choose_Array[data] & 8)     >> 3);
		AX12((AX_Choose_Array[data] & 16)    >> 4);
		AX11((AX_Choose_Array[data] & 32)    >> 5);
		AX10((AX_Choose_Array[data] & 64)    >> 6);
		AX9((AX_Choose_Array[data]  & 128)   >> 7);
		AX8((AX_Choose_Array[data]  & 256)   >> 8);
		AX7((AX_Choose_Array[data]  & 512)   >> 9);
		AX6((AX_Choose_Array[data]  & 1024)  >> 10);
		AX5((AX_Choose_Array[data]  & 2048)  >> 11);
		AX4((AX_Choose_Array[data]  & 4096)  >> 12);
		AX3((AX_Choose_Array[data]  & 8192)  >> 13);
		AX2((AX_Choose_Array[data]  & 16384) >> 14);
		AX1((AX_Choose_Array[data]  & 32768) >> 15);
}

/*
功能：行列扫描读取数据
输入：无
返回：返回16*16的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{
    for(uint16_t AY = 0; AY < 4; ++AY)
    {
			AY_Selection(AY);
			for(uint16_t AX = 0; AX < AX_NUM; ++AX)
			{
				AX_Selection(AX);				
				Hc4051Delay(ADC_DELAY_TIME);	
				adc_raw_value->value[AX][AY]      =  GetAdcValue(1) / 2;
				adc_raw_value->value[AX][AY +  4] =  GetAdcValue(0) / 2;
				adc_raw_value->value[AX][AY +  8] =  GetAdcValue(2) / 2;
				adc_raw_value->value[AX][AY + 12] =  GetAdcValue(3) / 2;
//				adc_raw_value->value[AX][AY]      =  255 * GetAdcValue(1) / 4096;
//				adc_raw_value->value[AX][AY +  4] =  255 * GetAdcValue(0) / 4096;
//				adc_raw_value->value[AX][AY +  8] =  255 * GetAdcValue(2) / 4096;
//				adc_raw_value->value[AX][AY + 12] =  255 * GetAdcValue(3) / 4096;
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
//				printf("%u \n", adc_raw_value->d_voltage[0][0]);				
				memset(adc_raw_value->sumAX, 0 , sizeof(adc_raw_value->sumAX));	
				flag_sum = 1;
		}
}			

void display_reampping(Hc4051AdcValue *adc_raw_value)
{
	uint8_t AY_display[AX_NUM] = {2,3,1,0, 6,7,5,4, 10,11,9,8, 14,15,13,12};
	for(uint8_t AX = 0; AX < AX_NUM; ++AX)
	{
		for(uint8_t AY = 0; AY < AY_NUM; ++AY)
		{
			adc_raw_value->voltage_display[AX][AY] = adc_raw_value->d_voltage[AX][AY_display[AY]];
		}
	}

	/*---------------Insole_display covert---------------*/
		adc_raw_value->insole_display[0][0] = adc_raw_value->voltage_display[14][0];
		adc_raw_value->insole_display[0][1] = adc_raw_value->voltage_display[14][3];	
		adc_raw_value->insole_display[1][0] = adc_raw_value->voltage_display[14][1];
		adc_raw_value->insole_display[1][1] = adc_raw_value->voltage_display[14][2];
		adc_raw_value->insole_display[2][0] = adc_raw_value->voltage_display[13][0];  
		adc_raw_value->insole_display[2][1] = adc_raw_value->voltage_display[13][3]; 
		adc_raw_value->insole_display[3][0] = adc_raw_value->voltage_display[13][1];
		adc_raw_value->insole_display[3][1] = adc_raw_value->voltage_display[13][2];
		adc_raw_value->insole_display[4][0] = adc_raw_value->voltage_display[15][0];
		adc_raw_value->insole_display[4][1] = adc_raw_value->voltage_display[15][3];
		adc_raw_value->insole_display[5][0] = adc_raw_value->voltage_display[15][1];
		adc_raw_value->insole_display[5][1] = adc_raw_value->voltage_display[15][2];	
	
}


