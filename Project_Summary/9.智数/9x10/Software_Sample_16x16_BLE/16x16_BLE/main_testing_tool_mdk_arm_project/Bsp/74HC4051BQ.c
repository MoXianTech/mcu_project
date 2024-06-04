#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>


Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (10 * 220) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}

void Hc4051IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
	
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, 
															GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3|
														  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8|
														  GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14 |
															GPIO_PIN_15);
	
	    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,
															GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
														  GPIO_PIN_8| GPIO_PIN_11| GPIO_PIN_12 | GPIO_PIN_15);
															
	

	
	GREEN_LED(0);
	
	ChooseEnlarge(3); 
}



/*
功能：选择分压电阻
输入：0~3对应的D电阻为：4.7K、100R、200R、1K
*/
void ChooseEnlarge(uint8_t data)
{
    MA(data & 0x01);
    MB((data & 0x02) >> 1);
}

/*
功能：选择行导通
输入：第几行
*/
void ChooseChanneR1R4(uint8_t data)
{
    CA(data & 0x01);
    CB((data & 0x02) >> 1);
}

/*

功能：选择列导通
输入：第几列 
*/
void ChooseCloumnChannel(uint16_t data)
{
	uint32_t Column_Choose_Array[16]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x00100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};
	
	C16(Column_Choose_Array[data] & 0x0001);
	C15((Column_Choose_Array[data] & 0x0002) >> 1);
	C14((Column_Choose_Array[data] & 0x0004) >> 2);
	C13((Column_Choose_Array[data] & 0x0008) >> 3);
	C12((Column_Choose_Array[data] & 0x0010) >> 4);
	C11((Column_Choose_Array[data] & 0x0020) >> 5);
	C10((Column_Choose_Array[data] & 0x0040) >> 6);
	C9((Column_Choose_Array[data] & 0x0080) >> 7);
	C8((Column_Choose_Array[data] & 0x0100) >> 8);
	C7((Column_Choose_Array[data] & 0x200) >> 9);
	C6((Column_Choose_Array[data] & 0x400) >> 10);
	C5((Column_Choose_Array[data] & 0x800) >> 11);
	C4((Column_Choose_Array[data] & 0x1000) >> 12);
	C3((Column_Choose_Array[data] & 0x2000) >> 13);
	C2((Column_Choose_Array[data] & 0x4000) >> 14);
	C1((Column_Choose_Array[data] & 0x8000) >> 15);

}

/*
功能：行列扫描读取数据
输入：无
返回：返回16*16的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{
    for(uint16_t R = 0; R < 4; ++R)
    {
			ChooseChanneR1R4(R);
		
			for(uint16_t C = 0; C < 16; ++C)
			{
				ChooseCloumnChannel(C);				
				Hc4051Delay(ADC_DELAY_TIME);
						
				adc_raw_value->raw_value[C][R     ] = GetAdcValue(1) * 50.0f;
				adc_raw_value->raw_value[C][R +  4] = GetAdcValue(0) * 50.0f;
				adc_raw_value->raw_value[C][R +  8] = GetAdcValue(2) * 50.0f;
				adc_raw_value->raw_value[C][R + 12] = GetAdcValue(3) * 50.0f;
			}
    }
	}

	
void ProcessADCvalue(Hc4051AdcValue *adc_raw_value)
{
		uint16_t Vref = 3.3 * 100;
		uint16_t flag_sum = 1;

		while(flag_sum)
		{					
				for(uint8_t AY = 0; AY < 16; ++AY)
				{
						for(uint8_t AX = 0; AX < 16; ++AX)
						{
								adc_raw_value->sumAX[AY] += adc_raw_value->raw_value[AX][AY];
						}
				}	
//				printf("fun/lin %s-%d: %u\n",__FUNCTION__,__LINE__, adc_raw_value->sumAX[0] );
				flag_sum = 0;						
		}
		
		while(flag_sum == 0)
		{
				
				for(uint8_t AY = 0; AY < 16; ++AY)
				{			
							for(uint8_t AX = 0; AX < 16; ++AX)
							{
									adc_raw_value->voltage[AX][AY] = (adc_raw_value->raw_value[AX][AY] * 255) / (Vref - (adc_raw_value->sumAX[AY] - adc_raw_value->raw_value[AX][AY]));					
							}
				} 
				
				memset(adc_raw_value->sumAX, 0 , sizeof(adc_raw_value->sumAX));	
				flag_sum = 1;
		}
//		printf("fun/lin %s-%d: %4u %4u %4u %4u \n", __FUNCTION__, __LINE__,adc_raw_value->voltage[0][0],adc_raw_value->value[0][0],adc_raw_value->voltage[1][0],adc_raw_value->value[1][0]);	
}				


void visual_remaping(Hc4051AdcValue *adc_raw_value)
{
	uint16_t AY_display[10] = {6,0,1,3,2,7,5,4,10,11}; 
	uint16_t AX_display[9]  = {15,14,13,12,11,10,9,8,7};
	
	for(uint8_t AX = 0; AX < 9; AX++)
	{
		for(uint16_t index = 0; index < 10; index++)
		{
			adc_raw_value->dispaly[AX][index] = adc_raw_value->voltage[AX_display[AX]][AY_display[index]];	
		}
	}
}

void swap(uint8_t* a, uint8_t* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

