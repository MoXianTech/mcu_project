/*************************************************************************
	> File Name: 74hc595.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/9/20 17:48:31
 ************************************************************************/

#include "stdint.h"
#define DS(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_0);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_0)						



void ic_74hc595_delay(uint32_t num);
void ic_74hc595_init(void);
void ic_74hc595_thread(void);
