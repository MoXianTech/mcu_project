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
#include <stdint.h>
#include "gd32e10x.h"
#include "systick.h"
#include "led.h"
#include "adc.h"
#include "uart.h"
#include "timer.h"
#include "74hc4051bq.h"
#include "flash.h"
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "oled.h"
#include "math.h"
#include "aid_agreement.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

#pragma pack (1)

#define LOG_PRINTF 0

#ifdef USART_DEF_921600
#define USART2_DEF 921600
#endif

#ifdef USART_DEF_115200
#define USART2_DEF 115200
#endif


typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[SENSOR_POS_X][SENSOR_POS_Y];
    uint16_t checksum;
} SerialFrame_t;

#pragma pack ()

bool timer3_interrupt = true;
uint32_t time_1ms = 0;

SerialFrame_t serial_frame;
process_handle_t process_handle = {0};

uint16_t CalChecksum(uint8_t * data, uint16_t len);

int main(void)
{
    bool led_flag = false;
    uint8_t count = 0;


    systick_config();
    led_init();
    adc_init();
    Hc4051IoInit();

#if 0
    nvic_irq_enable( TIMER3_IRQn, 2, 0 );
    timer_config(TIMER3, 1); //20ms
#endif


    delay_1ms(500);
    usart2_init(USART2_DEF);
    usart3_init(921600);

    while( 1 )
    {
        time_1ms ++;
        //LED闪烁

        if(time_1ms % 500 == 0)
        {
            led_flag = !led_flag;
            GREEN_LED(led_flag);
        }

        //50Hz速率采集第一快压感数据
        if(time_1ms % 1 == 0)
        {
            adc_value_read(&process_handle, count, SINGLE_VCC_READ);

            count ++;
            if (count == SENSOR_POS_X)
            {
                count = 0;
            }


            select_y_control_volt(&process_handle, count, SINGLE_VCC_READ);
        }

        //大约25Hz上传所有的数据
        if(time_1ms % 33 == 0)
        {
            //数据排列转换
            serial_frame.sof = 0x5aa5;
            serial_frame.tran_type = 0x01;
            serial_frame.len = sizeof(serial_frame) - 2;
            serial_frame.type = 0x01;

            adc_calculation_calibration_once(&process_handle, PROCESS_OUT_MODE);
            memcpy((uint8_t *)serial_frame.adc_value, (uint8_t *)process_handle.adc_cali_value, SENSOR_POS_X * SENSOR_POS_Y);
            serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);

#if 0
            printf("%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d\n",
                    GetAdcValue(0),
                    GetAdcValue(1),
                    GetAdcValue(2),
                    GetAdcValue(3),
                    GetAdcValue(4),
                    GetAdcValue(5),
                    GetAdcValue(6),
                    GetAdcValue(7),
                    GetAdcValue(8),
                    GetAdcValue(9),
                    GetAdcValue(10),
                    GetAdcValue(11)
                  );
#endif
            if (1)
            {
                usart_dma_send_data(USART_3_TR, (uint8_t *)&serial_frame, sizeof(serial_frame));
            }

            if (BLE_LINK_GET == RESET)
            {
                usart_dma_send_data(USART_2_TR, (uint8_t *)&serial_frame, sizeof(serial_frame));
            }
        }

        delay_1ms(1);
    }
}

void read_can0_data(can_receive_message_struct *receive_message)
{
    uint16_t count = 0;

    for (count = 0; count < 8; count ++)
    {
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
    read_can0_data(&receive_message);
}

void TIMER3_IRQHandler( void )
{
    if( SET == timer_interrupt_flag_get( TIMER3, TIMER_INT_FLAG_UP ) )
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear( TIMER3, TIMER_INT_FLAG_UP );

    }
}
