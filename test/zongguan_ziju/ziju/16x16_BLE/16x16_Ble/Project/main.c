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
#include "BLE.h"

//#define BLE_init

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

#pragma pack (1)

typedef struct {
    uint32_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint16_t adc_value[6];
    uint16_t checksum;
} SerialFrame_t;

#pragma pack ()

uint8_t printf_buffer[512] = {0};

bool timer3_interrupt = true;
uint64_t time_1ms = 0, end_time_1ms = 0, new_test_time = 0;;

Hc4051AdcValue temp_adc_value;

SerialFrame_t serial_frame;

uint16_t CalChecksum(uint8_t * data, uint16_t len);
FlagStatus gpio_input_bit_get(uint32_t gpio_periph,uint32_t pin);
void send_message_once(uint8_t command, uint16_t adc_value);
int main(void)
{
		uint16_t index = 0, key_value = 0, key_record = 0;
    systick_config();
    adc_init();
    Hc4051IoInit();
#ifdef BLE_init
		ble_init();
#else
		ble_gpio_init();
#endif
    usart2_init(115200);
    usart0_init(115200);
		AX_Selection(15);
		AX2(1);

    while(1)
    {
        time_1ms ++;
        //LED闪烁
        if(time_1ms % 500 == 0)
        {
            static bool led_flag = false;
            led_flag = !led_flag;
//            GREEN_LED(led_flag);
        }

        //50Hz速率采集第一快压感数据
        if(time_1ms % 1 == 0)
        {
            ReadAdcValue(&temp_adc_value);
//						ProcessADCvalue(&temp_adc_value);
        }

      //大约25Hz上传所有的数据
        if(time_1ms % 1 == 0)
        {
					

						for(uint8_t AY = 0; AY < 6; ++AY)
						{
							if (temp_adc_value.value[0][AY + 1] < 20)
							{
								serial_frame.adc_value[AY] = 0;
								continue;
							}
							serial_frame.adc_value[AY] = (4070.0 - temp_adc_value.value[0][AY + 1]) / temp_adc_value.value[0][AY + 1] * 100;
							if (serial_frame.adc_value[AY] > 5000)
								serial_frame.adc_value[AY] = 5000;

							
							if (index > 6)
								break;
						}

        }
				
				if (1)
				{
						key_value = temp_adc_value.value[0][0];
					
						if (key_value > 1000 && key_record < 1000)
						{
								end_time_1ms = time_1ms + 5;
								key_record = key_value;
						}

						if (key_value < 1000 && key_record > 1000)
						{
								end_time_1ms = time_1ms + 5;
								key_record = key_value;
						}

						if (end_time_1ms == time_1ms && key_value > 1000)
						{
								send_message_once(0x01, 0);
								GREEN_LED(0);
						}

						if (end_time_1ms == time_1ms && key_value < 1000)
						{

								send_message_once(0x02, 0);
								new_test_time = time_1ms + 10;
								GREEN_LED(1);
						}

						if (new_test_time == time_1ms && key_value < 1000)
						{
								send_message_once(0x00, 0);
								GREEN_LED(0);
								delay_1ms(50);
								GREEN_LED(1);
								delay_1ms(50);
								GREEN_LED(0);
								delay_1ms(50);
								GREEN_LED(1);
								delay_1ms(50);
								GREEN_LED(0);
								delay_1ms(50);
								GREEN_LED(1);
						}
				}

        delay_1ms(100);
    }
}

void send_message_once(uint8_t command, uint16_t adc_value)
{

		/*Serial is the data transmit to the upper PC*/
		serial_frame.sof = 0xaa555aa5;
		serial_frame.tran_type = command;
		serial_frame.len = sizeof(serial_frame) - 2;
		serial_frame.type = 0x01;

	


		serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
		usart_dma_send_data(USART_0_TR, (uint8_t *)&serial_frame, sizeof(serial_frame));
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

