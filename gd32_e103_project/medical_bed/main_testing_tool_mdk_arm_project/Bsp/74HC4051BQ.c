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
														  GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 );

	gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_10MHZ,  GPIO_PIN_12);
    
	GREEN_LED(0);
    YC_N_0(0);
    YC_N_1(1);
    YC_N_2(1);
    YC_N_3(1);

    YC_A(1);
    YC_B(0);
    YC_C(0);

    XC_A(0);
    XC_B(0);
    XC_C(1);
}

void select_x_control(uint8_t x_value)
{
    uint16_t y_default = 0x00;
    uint8_t hardware_pint = 0;

    hardware_pint |= (x_value & 0x01) << 2;
    hardware_pint |= (x_value & 0x02) << 1;
    hardware_pint |= (x_value & 0x04);

    y_default = gpio_output_port_get(GPIOB);

    y_default &= (hardware_pint & 0x3) << 3;
    gpio_port_write(GPIOB, y_default);

}

void select_y_control(uint8_t y_value)
{
    uint16_t y_default = 0x00;
    uint8_t hardware_pint = 0;

    hardware_pint |= (y_value & 0x01) << 2;
    hardware_pint |= (y_value & 0x02) << 1;
    hardware_pint |= (y_value & 0x04);

    y_default = gpio_output_port_get(GPIOB);
    y_default &= 0x0fff;
    switch(y_value)
    {
        case 0: case 1: case 2: case 3:
        case 4: case 5: case 6: case 7:
            y_default |= 0x8000;
            break;
        case 8: case 9: case 10: case 11:
        case 12: case 13: case 14: case 15:
            y_default |= 0x4000;
            break;
        case 16: case 17: case 18: case 19:
        case 20: case 21: case 22: case 23:
            y_default |= 0x2000;
            break;
        case 24: case 25: case 26: case 27:
        case 28: case 29: case 30: case 31:
            y_default |= 0x1000;
            break;
        default:
            break;
    }

    y_default |= (hardware_pint & 0x3);
    gpio_port_write(GPIOB, y_default);
}

/*
功能：行列扫描读取数据
输入：无
返回：返回16*16的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value)
{
		{
			Hc4051Delay(ADC_DELAY_TIME);

			adc_raw_value->value[0][0] = GetAdcValue(0) * 50.0f;
			adc_raw_value->value[0][1] = GetAdcValue(1) * 50.0f;
			adc_raw_value->value[0][2] = GetAdcValue(2) * 50.0f;
			adc_raw_value->value[0][3] = GetAdcValue(3) * 50.0f;
			adc_raw_value->value[0][4] = GetAdcValue(4) * 50.0f;
			adc_raw_value->value[0][5] = GetAdcValue(5) * 50.0f;
			adc_raw_value->value[0][6] = GetAdcValue(6) * 50.0f;
			adc_raw_value->value[0][7] = GetAdcValue(7) * 50.0f;
				
        }

}
