#ifndef __ADC_h
#define __ADC_h

#include "gd32f4xx.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "stdio.h"

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
void adc_config(void);
void adc_init(void);
void read_adc_value(void);
uint16_t GetAdcValue(uint8_t index);

void printf_adc_value(uint8_t *printf_buffer, uint16_t buffer_size);

#endif

