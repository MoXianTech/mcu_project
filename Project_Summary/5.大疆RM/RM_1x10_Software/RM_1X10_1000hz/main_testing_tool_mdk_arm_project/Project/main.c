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
#include "adc.h"
#include "uart.h"
#include "timer.h"
#include "74hc4051bq.h"
#include "flash.h"
#include "main.h"
#include "led.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

#pragma pack (1)
typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[10];
    uint16_t checksum;
} SerialFrame_t;
#pragma pack ()


bool timer3_interrupt = true;
uint32_t time_1ms = 0;

Hc4051AdcValue temp_adc_value;
SerialFrame_t serial_frame;

uint16_t CalChecksum(uint8_t * data, uint16_t len);

int main(void)
{

		systick_config();
    adc_init();
    Hc4051IoInit();
    usart0_init();
		led_gpio_init();
    while(1)
    {
        time_1ms ++;
        //LED闪烁
        if(time_1ms % 500 == 0)
        {
					led_spark();
        }

        //50Hz速率采集第一快压感数据
        if(time_1ms % 1 == 0)
        {
           ReadAdcValue(&temp_adc_value);
        }
				


      //大约25Hz上传所有的数据
        if(time_1ms % 1 == 0)
        {
						/*Serial is the data transmit to the upper PC*/
            serial_frame.sof = 0x5aa5;
            serial_frame.tran_type = 0x01;
            serial_frame.len = sizeof(serial_frame) - 2;
            serial_frame.type = 0x01;
						serial_frame.adc_value[0] = temp_adc_value.value[0];
						serial_frame.adc_value[1] = temp_adc_value.value[1];
						serial_frame.adc_value[2] = temp_adc_value.value[2];
						serial_frame.adc_value[3] = temp_adc_value.value[3];
						serial_frame.adc_value[4] = temp_adc_value.value[4];
						serial_frame.adc_value[5] = temp_adc_value.value[9];
						serial_frame.adc_value[6] = temp_adc_value.value[8];
						serial_frame.adc_value[7] = temp_adc_value.value[7];
						serial_frame.adc_value[8] = temp_adc_value.value[6];
						serial_frame.adc_value[9] = temp_adc_value.value[5];
						serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
				
						dma_send_data((uint8_t *)&serial_frame, sizeof(serial_frame));
        }
        delay_1ms(1);
    }
}

uint16_t CalChecksum(uint8_t * data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}



void TIMER3_IRQHandler( void )
{
    if( SET == timer_interrupt_flag_get( TIMER3, TIMER_INT_FLAG_UP ) )
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear( TIMER3, TIMER_INT_FLAG_UP );

    }
}








