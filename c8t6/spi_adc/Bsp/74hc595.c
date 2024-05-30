/*************************************************************************
	> File Name: 74hc595.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/9/20 17:48:24
 ************************************************************************/

#include  "74hc595.h"
#include "gd32e10x.h"
#include  <stdlib.h>
#include  <stdio.h>
#include  <stdint.h>
#include "systick.h"


#define DS(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_3);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_3)						

#define SHCP(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_4);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_4)						

#define STCP(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_5);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_5)			
		
inline void ic_74hc595_delay(uint32_t num)
{
    while(num --);
}

void ic_74hc595_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
}

void ic_74hc595_thread(void)
{
    static uint8_t io_value = 0, total_value = 0;
    if (io_value)
        DS(1);
    else
        DS(0);
		//if (total_value >= 128) return;
    io_value ++;
		total_value ++;


    if (io_value == 72) io_value = 0;


    STCP(0);
    SHCP(0);
    ic_74hc595_delay(1);
    SHCP(1);
    ic_74hc595_delay(1);
    SHCP(0);
    STCP(1);
    ic_74hc595_delay(1);
    STCP(0);
}

