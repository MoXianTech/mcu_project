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
//#include "BLT.h"
#include "i2c.h"
#include "i2c_XL9555.h"
#include "jiaota.h"



/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

	
bool timer3_interrupt = true;
uint32_t time_1ms = 0;

Hc4051AdcValue temp_adc_value;
SerialFrame_t serial_frame;
uint16_t CalChecksum(uint8_t * data, uint16_t len);
uint8_t match_value[24][10] = {0};

int main(void)
{
		bool led_flag = false;
		uint8_t count_ax = 0;
		uint8_t count_x = 0, count_y = 0;
		int8_t return_value = 0;
	
		systick_config();
    adc_init();
		led_init();
		RS2251_Init();	

    usart0_init(460800);
		usart2_init(460800);
	
		i2c_init(I2C_COM0, 400 * 1000);
		xl9555_init();
	
    while(1)
    {
        time_1ms ++;			
        //LED闪烁
        if(time_1ms % 500 == 0)
        {
            led_flag = !led_flag;
//            GREEN_LED(led_flag);

				}

        //100Hz速率采集第一快压感数据
        if(time_1ms % 1 == 0)
        {	
//					single_xl9555_io_set(count_ax);
					ReadAdcValue(&temp_adc_value, count_ax);
					
					count_ax ++;
					if(count_ax == 24)
					{
						ProcessADCvalue(&temp_adc_value, SENSOR_A, 16);
					}
					if(count_ax >= 25)
					{				
						ProcessADCvalue(&temp_adc_value, SENSOR_B, 16);
						count_ax = 0;	
					}
					xl9555_io_set(count_ax, 0);

				}

				//大约20Hz上传所有的数据
        if(time_1ms % 50 == 0)
        {
					/*DATA Package*/
					serial_frame.sof = 0x5aa5;
					serial_frame.tran_type = 0x01;
					serial_frame.len = sizeof(serial_frame) - 2;
					serial_frame.type = 0x01;
					/*48x24Data_Display*/ 

					memcpy((uint8_t *)serial_frame.adc_value,(uint8_t *)temp_adc_value.sensorA_voltage + 336, 10*24);
//					memcpy((uint8_t *)serial_frame.adc_value,(uint8_t *)temp_adc_value.sensorA_voltage , 24*24);
					for (count_x = 0; count_x < 10; count_x ++)
					{
						for (count_y = 0; count_y < 24; count_y ++)
						{
							match_value[count_y][count_x] = serial_frame.adc_value[count_x][count_y];
						}
					}

					serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
					usart_dma_send_data(USART_0_TR, (uint8_t *)&serial_frame, sizeof(serial_frame));
					usart_dma_send_data(USART_2_TR, (uint8_t *)&serial_frame, sizeof(serial_frame));
					
        }
				
				if(time_1ms % 500 == 0)
				{
					return_value = foot_alarm(match_value);
					if (return_value != 0)
					{
							GREEN_LED(0);
							if (return_value > 1)
								single_xl9555_io_set(47);
							else
								single_xl9555_io_reset(47);
					} else {
							GREEN_LED(1);
							single_xl9555_io_reset(47);
					}		
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



