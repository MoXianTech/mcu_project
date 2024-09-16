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
#include "uart.h"
#include "timer.h"
#include "flash.h"
#include "main.h"
#include "led.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */


//#define ENABLE_ADC_BOARD_PRINTF
#define CACHE_BUF_SIZE 2048
#define ADC_VALUE_NUM_LARGE 16 * 15 * 2


#pragma pack (1)
typedef struct {
    uint16_t adc_value_sensor_1[ADC_VALUE_NUM_LARGE];
    uint16_t sum_value;
    char adc_value_1[1300];
} sensor_1_single_frame_t;

typedef struct {
    char *current_process;
    char *X_distance;
    char *Y_distance;
    char *Z_distance;
    char *load;
    char *resistacne;
} equiment_data_pack_t;

#pragma pack ()

bool timer3_interrupt = true;
char str[64];
char *somma = ",";
uint8_t recv_cache_buf[CACHE_BUF_SIZE];


sensor_1_single_frame_t sensor_1_single_frame = {0};
equiment_data_pack_t equiment_data_pack = {0};

uint16_t pack_tail_index = 0;
uint16_t adc_value_pack_head_index = 0;
uint16_t adc_value_index = 0;


uint16_t CalChecksum(uint8_t * data, uint16_t len);

int main(void)
{
    bool led_flag = false;
    uint32_t time_1ms = 0;
    uint16_t usart0_len = 0, frame_usart0_len = 0, frame_adc_num = 0;
    uint16_t usart2_len = 0;
    uint16_t  search_count = 0, count;
    void *usb_handle = NULL;

    systick_config();
    led_init();

    usart0_init();  //115200
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



        if(time_1ms % 25 == 0)
        {
            if (!usart0_rx_probe())
            {
                usart0_len = USART0_GetDataCount();
                if(usart0_len > 128)
                {

                    for(count = 0; count < 32; count ++)
                    {
                        USART0_Recv(recv_cache_buf, 4);
                        if (recv_cache_buf[0] == 0xA5 && recv_cache_buf[1] == 0x5A && recv_cache_buf[2] == 0x55 && recv_cache_buf[3] == 0xAA)
                        {
                            USART0_Recv(recv_cache_buf, 4);
                            frame_usart0_len = recv_cache_buf[2] << 8 | recv_cache_buf[1];
                            frame_usart0_len -= 6;
                            frame_adc_num = frame_usart0_len / 2 - 1;
                            break;
                        } else {

                        }
                    }

                    memset((uint8_t *)sensor_1_single_frame.adc_value_1, 0x00, sizeof(sensor_1_single_frame.adc_value_1));

                    do {
                        usart0_len = USART0_GetDataCount();
                    }
                    while(!(usart0_len >= frame_usart0_len));

                    USART0_Recv((uint8_t *)sensor_1_single_frame.adc_value_sensor_1, frame_usart0_len);

                    for(uint16_t convert_count = 0; convert_count < frame_adc_num; ++ convert_count)
                    {
                        sprintf(str, ",%d", sensor_1_single_frame.adc_value_sensor_1[convert_count]);
                        strcat(sensor_1_single_frame.adc_value_1, str);
                        memset(str, 0x00, sizeof(str));
                    }

                    sprintf(str, ",debug info adc_num %d frame_len %d\n\0", frame_adc_num, frame_usart0_len);
                    strcat(sensor_1_single_frame.adc_value_1, str);
#ifdef ENABLE_ADC_BOARD_PRINTF
                    usb_send_buffer(usb_handle, (uint8_t *)sensor_1_single_frame.adc_value_1, strlen(sensor_1_single_frame.adc_value_1));
                    delay_1ms(5);
#endif

                    memset(recv_cache_buf, 0x00, sizeof(recv_cache_buf));
                }

            }
        }


        if(time_1ms % 50 == 0)
        {
            if (!usart2_rx_probe())
            {
                usart2_len = USART2_GetDataCount();
                if (usart2_len > 32)
                {
                    memset(recv_cache_buf, 0x00, sizeof(recv_cache_buf));
                    USART2_Recv(recv_cache_buf, usart2_len);

#if 1
                    for(search_count = 0; search_count < usart2_len; ++search_count)
                    {
                        if(recv_cache_buf[search_count] == 0x0D && recv_cache_buf[search_count + 1] == 0x0A)
                        {
                            pack_tail_index = search_count;
                            recv_cache_buf[search_count] = '\0';
                            break;
                        }
                    }
#endif

                    memcpy((char *)recv_cache_buf + search_count, (char *)sensor_1_single_frame.adc_value_1, strlen(sensor_1_single_frame.adc_value_1));
                    sprintf((char *)recv_cache_buf,"%s press_len %d serch %d total_len %d\r\n", recv_cache_buf, usart2_len, search_count, strlen((char *)recv_cache_buf));
#ifndef ENABLE_ADC_BOARD_PRINTF
                    usb_send_buffer(usb_handle,(uint8_t *)recv_cache_buf, strlen((char *)recv_cache_buf));
                    delay_1ms(5);
#endif

                    pack_tail_index = 0;
                }


#if 0
                equiment_data_pack.current_process = strtok((char *)recv_cache_buf,somma);
                equiment_data_pack.X_distance = strtok(NULL, somma);
                equiment_data_pack.Y_distance = strtok(NULL, somma);
                equiment_data_pack.Z_distance = strtok(NULL, somma);
                equiment_data_pack.load = strtok(NULL,somma);
                equiment_data_pack.resistacne = strtok(NULL,somma);

                strcpy((char *)serial_frame.load,equiment_data_pack.load );
                strcpy((char *)serial_frame.resistance,equiment_data_pack.resistacne);

                printf("%s %s %s %s %s %s %d\n",equiment_data_pack.current_process,
                        equiment_data_pack.X_distance,
                        equiment_data_pack.Y_distance,
                        equiment_data_pack.Z_distance,
                        equiment_data_pack.load,
                        equiment_data_pack.resistacne,
                        strlen(equiment_data_pack.X_distance));
#endif
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

