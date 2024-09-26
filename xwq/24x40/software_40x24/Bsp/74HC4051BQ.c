#include "74hc4051bq.h"
#include  "process_r_value.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>
#include "i2c_XL9555.h"

#define MIN_RAW_VALUE	1000
#define MAX_RAW_VALUE	800000


#define ADC_DELAY_TIME (6 * 220) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}

/*
   功能：Init
   输入：none
   */

void led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 ); //LED_G
    GREEN_LED(0);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9 );
    gpio_bit_reset(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
}

void ref_gpio_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}

void RS2251_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}

/*
   功能：AY SCAN
   输入：data 0 -> 7
   */
void Choose_AX_Channel(uint8_t data)
{
    C_A( data & 0x01);
    C_B((data & 0x02) >> 1);
    C_C((data & 0x04) >> 2);
}


uint8_t range_switching(uint32_t range)
{

    static uint32_t r_pressure = 4000;
    uint8_t ret = 0;

    if(range >= MAX_RAW_VALUE )
        ret = 0;
    //ret = (255*300/range - 7);
    else
    {
        ret = 255*r_pressure/(r_pressure+range);
    }
    return ret;
}



//uint8_t range_switching(uint32_t range)
//{
//	uint8_t out = 0;
//	//out = 4000*255/(range+4000);
//	out = (1.0f/range-1.0f/MAX_RAW_VALUE)*255/(1.0f/MIN_RAW_VALUE-1.0f/MAX_RAW_VALUE);
//	//out = (uint64_t)((MAX_RAW_VALUE-range)*255*MAX_RAW_VALUE*MIN_RAW_VALUE/1000000)/((MAX_RAW_VALUE-MIN_RAW_VALUE)*(MAX_RAW_VALUE*range)/1000000);
//	return out;
//}

/*
   功能：行列扫描读取数据
   输入：无
   返回：返回32*64的数据
   */
uint8_t a = 0;
void ReadAdcValue(Hc4051AdcValue *adc_raw_value,uint8_t ay_index,RESISTACE_NAME resistace_name)
{
    static uint8_t B_adc_rank[8] = {3,0,1,2,4,6,7,5};

    if(resistace_name == RAW_RESISTACE)
    {
        for(uint8_t ax = 0; ax < 8; ++ax)
        {
            Choose_AX_Channel(B_adc_rank[ax]);
            Hc4051Delay(ADC_DELAY_TIME);

            adc_raw_value->sensorB_raw_value[ax]				= 	GetAdcValue(0);
            adc_raw_value->sensorB_raw_value[(ax +  8)]	= GetAdcValue(1);
            adc_raw_value->sensorB_raw_value[(ax + 16)]	= GetAdcValue(2);
            adc_raw_value->sensorB_raw_value[(ax + 24)]  = GetAdcValue(3);
            adc_raw_value->sensorB_raw_value[(ax + 32)]  = GetAdcValue(4);
        }

#if 0
        if(23-rankB_Y[ay_index] == 0)
        {
            a = 1;
        }
        for(uint8_t ax = 0; ax < AX_NUM; ax++)
        {
            adc_raw_value->sensorB_voltage[ax][23-rankB_Y[ay_index]] = range_switching(adc_raw_value->sensorB_cal_r_value_output[ax]);
            adc_raw_value->sensorB_cal_r_output[ax][23-rankB_Y[ay_index]] = adc_raw_value->sensorB_cal_r_value_output[ax];
        }
#endif

    }
    else
    {
        for(uint8_t ax = 0; ax < 8; ++ax)
        {
            Choose_AX_Channel(B_adc_rank[ax]);
            Hc4051Delay(ADC_DELAY_TIME);

            adc_raw_value->sensorB_ref_value[ax] = GetAdcValue(0);
            adc_raw_value->sensorB_ref_value[(ax+8)] = GetAdcValue(1);
            adc_raw_value->sensorB_ref_value[(ax+16)] = GetAdcValue(2);
            adc_raw_value->sensorB_ref_value[(ax+24)] = GetAdcValue(3);
            adc_raw_value->sensorB_ref_value[(ax+32)] = GetAdcValue(4);

        }
    }


}



