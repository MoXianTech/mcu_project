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
	
			gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);
	
	    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
																														GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |
																														GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15 );
	GREEN_LED(0);
	ChooseEnlarge(1); 
}



/*
功能：选择分压电阻
输入：0~3对应的D电阻为：4.7K、10K、200R、1K
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
void ChooseColumnChannel(uint32_t data)
{

	uint32_t Column_Choose_Array[24]={0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
																	  0x00100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
																		0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000};
	
	C1(Column_Choose_Array[data] & 0x0001);
	C2((Column_Choose_Array[data] & 0x0002) >> 1);
	C3((Column_Choose_Array[data] & 0x0004) >> 2);
	C4((Column_Choose_Array[data] & 0x0008) >> 3);
	C5((Column_Choose_Array[data] & 0x0010) >> 4);
	C6((Column_Choose_Array[data] & 0x0020) >> 5);
	C7((Column_Choose_Array[data] & 0x0040) >> 6);
	C8((Column_Choose_Array[data] & 0x0080) >> 7);
	C9((Column_Choose_Array[data] & 0x0100) >> 8);
	C10((Column_Choose_Array[data] & 0x200) >> 9);
	C11((Column_Choose_Array[data] & 0x400) >> 10);
	C12((Column_Choose_Array[data] & 0x800) >> 11);
	C13((Column_Choose_Array[data] & 0x1000) >> 12);
	C14((Column_Choose_Array[data] & 0x2000) >> 13);
	C15((Column_Choose_Array[data] & 0x4000) >> 14);
	C16((Column_Choose_Array[data] & 0x8000) >> 15);																	
	C17((Column_Choose_Array[data] & 0x10000) >> 16);
	C18((Column_Choose_Array[data] & 0x20000) >> 17);
	C19((Column_Choose_Array[data] & 0x40000) >> 18);
	C20((Column_Choose_Array[data] & 0x80000) >> 19);
	C21((Column_Choose_Array[data] & 0x100000) >> 20);
	C22((Column_Choose_Array[data] & 0x200000) >> 21);
	C23((Column_Choose_Array[data] & 0x400000) >> 22);
	C24((Column_Choose_Array[data] & 0x800000) >> 23);

}

/*
功能：行列扫描读取数据
输入：无
返回：返回1*24的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{
			
	for(uint16_t C = 0; C < 24; ++C)
			{
				ChooseColumnChannel(C);
					
				Hc4051Delay(ADC_DELAY_TIME);
						
#if 0
			GetAdcValue(1);
			GetAdcValue(2);
			GetAdcValue(3);
			GetAdcValue(4);
#endif
//			adc_raw_value.value[C][15 - R] = GetAdcValue(1) * 1000.0f;
//			adc_raw_value.value[C][11 - R] = GetAdcValue(2) * 1000.0f;
//			adc_raw_value.value[C][R] = GetAdcValue(3) * 1000.0f;
//			adc_raw_value.value[C][R + 4] = GetAdcValue(4) * 1000.0f;
	

			adc_raw_value->value[C][0] = GetAdcValue(0) * 50.0f;

			
			//限制压感数据不超过2.55V
//			if(adc_raw_value.value[C][R] > 255) adc_raw_value.value[C][R] = 255;
//			if(adc_raw_value.value[C][R + 4] > 255) adc_raw_value.value[C][R + 4] = 255;
//			if(adc_raw_value.value[C][R + 8] > 255) adc_raw_value.value[C][R + 8] = 255;
//			if(adc_raw_value.value[C][R + 12] > 255) adc_raw_value.value[C][R + 12] = 255;
//			}
  
//		printf("fun/lin %s-%d: %f %f %f %f\n", __FUNCTION__,
//																					 __LINE__,
//																					 GetAdcValue(0),
//																					 GetAdcValue(1),
//																					 GetAdcValue(2),
//																					 GetAdcValue(3)
//					);
//					printf("fun/lin %s-%d: %f  %u \n", __FUNCTION__,
//																					 __LINE__,
//																					 GetAdcValue(0)
//																					 );
	}
}
