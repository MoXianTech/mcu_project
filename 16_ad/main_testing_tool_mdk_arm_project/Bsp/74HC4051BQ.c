#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include "oled.h"

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
	
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
														  GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
														  GPIO_PIN_8 | 
														  GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_15 );
	
	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,    GPIO_PIN_12 | GPIO_PIN_13);
	
	RED_LED(0);
	GREEN_LED(0);
	
	ChooseEnlarge(0x03); //1k
}

uint8_t GetSwStatus(void)
{	
	uint16_t R_[4] = {56, 100, 200, 1000};
	uint8_t status = 0;
	static uint8_t last_status = 0;
	
	uint8_t bit1 = gpio_input_bit_get(GPIOB, GPIO_PIN_12);//B BIT1
	uint8_t bit0 = gpio_input_bit_get(GPIOB, GPIO_PIN_13);//A BIT0
	status = bit1 << 1 | bit0;
	
	if(status != last_status)
	{
		OLED_ShowStr(30, 1, "R:", 16);
		OLED_ShowNum(50, 1, R_[status], 4, 16);
		OLED_ShowStr(85, 1, "R", 16);
	}
	last_status = status;
	return status;
}
/*
功能：选择分压电阻
输入：0~3对应的D电阻为：56R、100R、200R、1K
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
void ChooseChanneC1C8(uint8_t data)
{
    A1(data & 0x01);
    B1((data & 0x02) >> 1);
    C1((data & 0x04) >> 2);
	EN1(0);
	EN2(1);
}

/*
功能：选择列导通
输入：第几列
*/
void ChooseChanneC9C16(uint8_t data)
{
    A1(data & 0x01);
    B1((data & 0x02) >> 1);
    C1((data & 0x04) >> 2);
	EN1(1);
	EN2(0);
}

/*
功能：行列扫描读取数据
输入：无
返回：返回16*16的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{
    uint16_t AC = 0;
    for(uint16_t R = 0; R < 4; ++R)
    {
		ChooseChanneR1R4(R);
		for(uint16_t C = 0; C < 1; ++C)
		{
			if(AC < 8)
			{
				ChooseChanneC1C8(AC);
			}
			else
			{
				ChooseChanneC9C16(AC - 8);
			}
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
			
			adc_raw_value->value[C][R] = GetAdcValue(1) * 50.0f;
			adc_raw_value->value[C][R + 4] = GetAdcValue(2) * 50.0f;
			adc_raw_value->value[C][R + 8] = GetAdcValue(3) * 50.0f;
			adc_raw_value->value[C][R + 12] = GetAdcValue(4) * 50.0f;
			
			//限制压感数据不超过2.55V
//			if(adc_raw_value.value[C][R] > 255) adc_raw_value.value[C][R] = 255;
//			if(adc_raw_value.value[C][R + 4] > 255) adc_raw_value.value[C][R + 4] = 255;
//			if(adc_raw_value.value[C][R + 8] > 255) adc_raw_value.value[C][R + 8] = 255;
//			if(adc_raw_value.value[C][R + 12] > 255) adc_raw_value.value[C][R + 12] = 255;
		}
    }

}
