/*!
  \file  main.c
  \brief ADC0 regular channel with DMA

  \version 2017-12-26, V1.0.0, firmware for GD32E10x
  */

/*
   Copyright (c) 2017, GigaDevice Semiconductor Inc.

   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification, 
   are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.
   3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software without 
   specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
   OF SUCH DAMAGE.
   */

#include "gd32e10x.h"
#include "systick.h"
#include <stdio.h>
#include "uart.h"
#include "main.h"
#include "ebus.h"
#include "led.h"

uint32 time_1ms = 0;
uint8  printf_buffer[30];

int main(void)
{
		nvic_vector_table_set( NVIC_VECTTAB_FLASH , 0x2000);  //设置中断向量表偏移
		__set_PRIMASK(0);  	//开启总中断
		rcu_periph_reset_disable(RCU_USART0RST);  //bootloader中用到了串口0，所以重置一下
		usart0_init();			//重新初始化串口，注意bootloader里的串口使能了中断，如果app不需要用到串口0则需要把串口0的中断失能。
		eBus_init();
		led_init();
	  systick_config();

    while( 1 )
    {
				time_1ms++;
				
				if(time_1ms % 1000 == 0)
				{
						static bool led_flag = false;
            led_flag = !led_flag; 
            GREEN_LED(led_flag);
				}
				
				if(time_1ms % 10 == 0)
				{
					bootloader_check(1);
				}
				
				if(time_1ms % 500 == 0)
				{
					sprintf((char *)printf_buffer,"下载成功");
					dma_send_data((uint8_t *)&printf_buffer, sizeof(printf_buffer));
				}
				
				delay_1ms(1);

    }
}
