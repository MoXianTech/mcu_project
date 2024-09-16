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

#include <stdlib.h>
#include <stdio.h>
#include "gd32e10x.h"
#include "systick.h"
#include "adc.h"
#include "uart.h"
#include "timer.h"
#include "74hc4051bq.h"
#include "flash.h"
#include "main.h"
#include "oled.h"
#include "math_filter.h"
#include "car_driver.h"
#include "led.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */




#define LOG_PRINTF 0
#define CACHE_BUF_SIZE 1300

#define SERIAL_PACK_LEN_USART2 (8 + 15 * 16)  //HEAD->sensor_1
#define SERIAL_PACK_LEN_USART0 (8 + 32 * 24)
#define SERIAL_PACK_LEN_USART1 (8 + 36 * 24)

#pragma pack (1)
typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
	  uint8_t adc_value[83][24]; //83x24
    uint16_t checksum;
} SerialFrame_t;

typedef struct {
		uint8_t adc_value_sensor_3[32][24]; 
} sensor_1_single_frame_t;

typedef struct {
		uint8_t adc_value_sensor_2[36][24];
} sensor_2_single_frame_t;

typedef struct {
		uint8_t adc_value_sensor_3[16][15];
} sensor_3_single_frame_t;

#pragma pack ()

bool timer3_interrupt = true;

SerialFrame_t serial_frame;
sensor_1_single_frame_t sensor_1_single_frame = {0};
sensor_2_single_frame_t sensor_2_single_frame = {0};
sensor_3_single_frame_t sensor_3_single_frame = {0};
process_handle_t process_handle = {0};

uint16_t CalChecksum(uint8_t * data, uint16_t len);
uint32_t do_crc_table(unsigned char *ptr, int32_t len);
uint8_t cache_buf[CACHE_BUF_SIZE];

int main(void)
{
    bool led_flag = false;
    uint32_t time_1ms = 0;
    uint16_t usart0_len = 0;
    uint16_t usart1_len = 0;
	  uint16_t usart2_len = 0;
    void *usb_handle = NULL;
		
		uint16_t data_head_index = 0;

    systick_config();
    led_init();

    usart0_init();  //460800
    usart1_init();  //460800
    usart2_init();  //115200
    usb_init(&usb_handle);

    while( 1 )
    {
        time_1ms ++;
        if(time_1ms % 500 == 0)
        {
            led_flag = !led_flag;
            GREEN_LED(led_flag);
        }

        if(time_1ms % 20 == 0)
        {

            serial_frame.sof = 0x5aa5;
            serial_frame.tran_type = 0x01;
            serial_frame.len = sizeof(serial_frame) - 2;
            serial_frame.type = 0x01;

            serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
						usb_send_buffer(usb_handle, (uint8_t *)&serial_frame, sizeof(serial_frame));

        }

				
        if(time_1ms % 5 == 0)
        {
			      if (!usart0_rx_probe())
            {

							usart0_len = USART0_GetDataCount();	
							USART0_Recv(cache_buf, SERIAL_PACK_LEN_USART0);

							if(usart0_len >= sizeof(sensor_1_single_frame_t))
							{
								
								for(uint16_t count = 0; count < 30; ++count)
								{
									if(cache_buf[count] == 0xa5 && cache_buf[count + 1] == 0x5a)
									{
										data_head_index = count;		
									}	
								}
								
								memcpy((uint8_t *)sensor_1_single_frame.adc_value_sensor_3,(uint8_t *)(cache_buf + 6 + data_head_index), 32 * 34 );
								memcpy((uint8_t *)serial_frame.adc_value,(uint8_t *)sensor_1_single_frame.adc_value_sensor_3, 32  * 24);
							}
							memset(cache_buf, 0x00, sizeof(cache_buf));
						}
					
														
						if (!usart1_rx_probe())
            {

							usart1_len = USART1_GetDataCount();	
							USART1_Recv(cache_buf, SERIAL_PACK_LEN_USART1);
							
							if(usart1_len >= sizeof(sensor_2_single_frame_t))
							{
								
								for(uint16_t count = 0; count < 30; ++count)
								{
									if(cache_buf[count] == 0xa5 && cache_buf[count + 1] == 0x5a)
									{
										data_head_index = count;		
									}	
								}	
								memcpy((uint8_t *)sensor_2_single_frame.adc_value_sensor_2, (uint8_t *)(cache_buf + 6 + data_head_index), 36 * 24);
								memcpy((uint8_t *)(serial_frame.adc_value) + 32 * 24,(uint8_t *)sensor_2_single_frame.adc_value_sensor_2, 36 * 24);
							}
							memset(cache_buf, 0x00, sizeof(cache_buf));
           }
					
					
					  if (!usart2_rx_probe())
            {
							usart2_len = USART2_GetDataCount();	
							USART2_Recv(cache_buf, SERIAL_PACK_LEN_USART2);

							if(usart2_len >= sizeof(sensor_3_single_frame_t))
							{
								
								for(uint16_t count = 0; count < 30; ++count)
								{
									if(cache_buf[count] == 0xa5 && cache_buf[count + 1] == 0x5a)
									{
										data_head_index = count;		
									}	
								}	
								memcpy((uint8_t *)sensor_3_single_frame.adc_value_sensor_3,(uint8_t *)cache_buf + 6 + data_head_index, 16 * 15);

								for(uint8_t ax = 0; ax < 16; ++ax)
								{
										for(uint8_t ay = 0; ay < 15; ++ay)
										{
											serial_frame.adc_value[ax + 32 + 36][ay + 5] = sensor_3_single_frame.adc_value_sensor_3[ax][ay];
										}
								}		
							}
								memset(cache_buf, 0x00, sizeof(cache_buf));
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


void CAN0_RX0_IRQHandler(void)
{
    static bool led_flag_red = false;
    led_flag_red = !led_flag_red;
    can_receive_message_struct receive_message;

    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
}

void TIMER3_IRQHandler( void )
{
    if( SET == timer_interrupt_flag_get( TIMER3, TIMER_INT_FLAG_UP ) )
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear( TIMER3, TIMER_INT_FLAG_UP );

    }
}
