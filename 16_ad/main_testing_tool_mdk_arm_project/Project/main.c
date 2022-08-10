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
#include "can.h"
#include "i2c.h"
#include "oled.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

#pragma pack (1)

#define LOG_PRINTF 0

typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[32];
    uint16_t checksum;
} SerialFrame_t;

#pragma pack ()

typedef struct 
{
    uint8_t adc_value[8][16][16];
} C128R16Value_t;

typedef struct 
{
    uint8_t adc_value[64][32];
} C64R32Value_t;

uint8_t printf_buffer[512] = {0};

C128R16Value_t c128_r16_value;
C64R32Value_t c64_r32_value;

bool timer3_interrupt = true;
uint32_t time_1ms = 0;

Hc4051AdcValue temp_adc_value;
SerialFrame_t serial_frame;


void C128R16ToC64R32(void);
uint16_t CalChecksum(uint8_t * data, uint16_t len);

int main(void)
{
    systick_config();
    adc_init();
    Hc4051IoInit();
    //	nvic_irq_enable( TIMER3_IRQn, 2, 0 );
    //	timer_config(TIMER3, 1); //20ms

    usart0_init();
    can0_init();

    IIC_GPIO_Init();
    OLED_Init();
    RED_LED(0);
    while( 1 )
    {
        time_1ms ++;
        //LED闪烁
        if(time_1ms % 500 == 0)
        {
            static bool led_flag = false;
            led_flag = !led_flag;
            GREEN_LED(led_flag);
        }
        //设置分压电阻
        if(time_1ms % 500 == 0)
        {
            ChooseEnlarge(GetSwStatus());
        }

        //50Hz速率采集第一快压感数据
        if(time_1ms % 20 == 0)
        {
            ReadAdcValue(&temp_adc_value);
            if (LOG_PRINTF)
                sprintf((char *)printf_buffer, "adc_value %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
                        temp_adc_value.value[1][0],
                        temp_adc_value.value[1][1],
                        temp_adc_value.value[0][2],
                        temp_adc_value.value[0][3],
                        temp_adc_value.value[0][4],
                        temp_adc_value.value[0][5],
                        temp_adc_value.value[0][6],
                        temp_adc_value.value[0][7],
                        temp_adc_value.value[0][8],
                        temp_adc_value.value[0][9],
                        temp_adc_value.value[0][10],
                        temp_adc_value.value[0][11],
                        temp_adc_value.value[0][12],
                        temp_adc_value.value[0][13],
                        temp_adc_value.value[0][14],
                        temp_adc_value.value[0][15]
                       );
        }


        //大约25Hz上传所有的数据
        if(time_1ms % 20 == 0)
        {
            //数据排列转换
            serial_frame.sof = 0x5aa5;
            serial_frame.tran_type = 0x01;
            serial_frame.len = sizeof(serial_frame) - 2;
            serial_frame.type = 0x01;

            for(int i = 0; i < 16; ++i)
            {
                serial_frame.adc_value[i] = temp_adc_value.value[0][i];
            }

            for(int i = 16; i < 32; i ++)
            {
                serial_frame.adc_value[i] = temp_adc_value.value[1][i - 16];
            }
            serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);

            if (LOG_PRINTF)
                dma_send_data((uint8_t *)&printf_buffer, sizeof(printf_buffer));
            else
                dma_send_data((uint8_t *)&serial_frame, sizeof(serial_frame));
        }
        delay_1ms(1);
    }
}

void read_can0_data(can_receive_message_struct *receive_message)
{
    uint16_t count = 0;

    for (count = 0; count < 8; count ++)
    {
        temp_adc_value.value[1][count]= receive_message->rx_data[count];
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
    RED_LED(led_flag_red);
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
