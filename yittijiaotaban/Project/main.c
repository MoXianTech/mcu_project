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
//#include "uart.h"
#include "74hc4051bq.h"
#include "main.h"
#include "math_filter.h"
#include "led.h"
#include "74hc595.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */



#define LOG_PRINTF 0

#pragma pack (1)
typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
	  uint8_t adc_cali_value[POS_X][POS_Y];
    uint16_t checksum;
} SerialFrame_t;

#pragma pack ()

process_handle_t process_handle = {0};
SerialFrame_t serial_frame;

uint16_t CalChecksum(uint8_t * data, uint16_t len);
uint32_t do_crc_table(unsigned char *ptr, int32_t len);
int main(void)
{
    bool led_flag = false;

    uint64_t time_1ms = 0;
    uint8_t count_x = 0;
    void *usb_handle = NULL;
    process_handle.x_max = POS_X;
    process_handle.y_max = POS_Y;
    systick_config();

    delay_1ms(2000);
    adc_init();
    led_init();

    //    usart0_init();
    //    usart1_init();

    usb_init(&usb_handle);
    rs2251IoInit();
    ic_74hc595_init();
    delay_1ms(100);


    while( 1 )
    {
        time_1ms ++;
        if(time_1ms % 500 == 0)
        {
            led_flag = !led_flag;
            GREEN_LED(led_flag);
            //i2c_thread_task();

        }

        if(time_1ms % 1 == 0)
        {
            ic_74hc595_thread();

            adc_value_read(&process_handle, count_x);

            count_x ++;
            if (count_x == 72)
                count_x = 0;
        }

#if 1
        if(time_1ms % 72 == 0)
        {

            {
                serial_frame.sof = 0x5aa5;
                serial_frame.tran_type = 0x01;
                serial_frame.len = sizeof(serial_frame);
                serial_frame.type = 0x01;

                adc_calculation_calibration_once(&process_handle, (uint8_t *)serial_frame.adc_cali_value, true);
								serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
                
                usb_send_buffer(usb_handle, (uint8_t *)&serial_frame, sizeof(serial_frame));
            }
        }
#endif
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
