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


#define DS_B(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_2);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_2)						

#define SHCP_B(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_0);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_0)						

#define STCP_B(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_1);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_1)
		
inline void ic_74hc595_delay(uint32_t num)
{
    while(num --);
}

void ic_74hc595_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
		rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 );

		
		ic_74hc595_clean();
}

void ic_74hc595_io_write(uint8_t io_num)
{
	
		DS_B(1);
		SHCP_B(1);
		ic_74hc595_delay(1);
		SHCP_B(0);
	
		for(uint8_t i = 0; i < io_num; i++)
    {
			DS_B(0);
    	SHCP_B(1);
    	ic_74hc595_delay(1);
	  	SHCP_B(0);
	  	ic_74hc595_delay(1);
	  }
	  STCP_B(1);
	  ic_74hc595_delay(1);
		STCP_B(0);
}

void ic_74hc595_clean(void)
{
		DS_B(0);
		SHCP_B(0);
		STCP_B(0);
		
		for(uint8_t i = 0; i < IO_MAX_NUM; i++)
		 {     
					SHCP_B(1);
					ic_74hc595_delay(1);

          //变为低电平，为下次准备
					SHCP_B(0);
					ic_74hc595_delay(1);
		 }  
		 
		 STCP_B(1);
		 ic_74hc595_delay(1);
		 STCP_B(0);
}



//void ic_74hc595_thread()
//{
//    static uint8_t io_value = 0, total_value = 0;
//	
//		ic_74hc595_io_write(io_value);
//    io_value ++;
//    if (io_value == 30) io_value = 0;
//}

