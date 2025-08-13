#ifndef __ADC_h
#define __ADC_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"

#define cumulative_num 8
#define adc_channel_num 8

//extern uint16_t  adc_v[adc_channel_num];
//extern volatile uint16_t adc_value1[8];
//extern uint16_t adc_value[8];//ADC采样值
//extern uint16_t Vol_Value[3];//电压值
void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void adc_config(void);
void adc_init(void);
void read_adc_value(void);
uint16_t GetAdcValue(uint8_t index);
uint8_t return_adc_value(void);
float rerturn_adc_out_get(void);
void adc_5init(void);
uint16_t return_controller_key(void);
#endif

