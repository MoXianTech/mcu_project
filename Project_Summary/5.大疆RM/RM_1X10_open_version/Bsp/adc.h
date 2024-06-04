#ifndef __ADC_h
#define __ADC_h

#define ADC_NUM 10

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void adc_config(void);
void adc_init(void);

uint16_t GetAdcValue(uint8_t index);
#endif

