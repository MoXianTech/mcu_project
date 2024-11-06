/*!
  \file    main.c
  \brief   GPIO running led demo

  \version 2022-04-26, V2.0.0, demo for GD32F4xx
  */

/*
   Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx.h"
#include "systick.h"

#include "led.h"
#include "spi.h"
#include "lcd.h"
#include "main.h"
#include "Filter.h"
#include "adc.h"
#include "74hc595.h"
#include "adc.h"
#include "rs2251.h"


#pragma pack (1)
typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_cali_value[POS_X][POS_Y];
    uint16_t checksum;
} SerialFrame_t;

typedef struct {
    uint8_t adc_value[POS_X][POS_Y];
} Struct_Adc;

#pragma pack ()

SerialFrame_t serial_frame;

//#define MEAN_SIZE 10
//Struct_Adc mean_temp_buffer[10] = {0};
//Struct_Adc mean_buffer = {0};
Struct_Adc spi_adc_buffer = {0};
uint32_t time_ms =0;
uint8_t count_x = 0;
uint32_t spi_speed = 1;
uint32_t spi_cnt = 0;

//void get_mean_value(Struct_Adc *buffer, Struct_Adc*mean_buffer,uint8_t mean_count);
//void get_filter_value(Struct_Adc *source_buffer,Struct_Adc *target_buffer);
uint16_t CalChecksum(uint8_t * data, uint16_t len);
uint32_t do_crc_table(unsigned char *ptr, int32_t len);
uint8_t channel_count = 0x00;
process_handle_t process_handle = {0};
int8_t scan_bit_num = 0x00;
int main(void)
{
    bool led_flag = false;
    void *usb_handle = NULL;


    systick_config();
    led_init();
    ic_74hc595_init(SCAN_HIGH_LEVEL);
    rs2251_io_init();
    adc_init();
    LCD_init();
    usb_init(&usb_handle);
    delay_1ms(400);
    process_handle.y_max = POS_Y;
    process_handle.x_max = POS_X;
    ic_74hc595_set_io_scan(0);

    while(1)
    {
        time_ms++;

        if(time_ms % 500 == 0)
        {
            led_flag = !led_flag;
            GREEN_LED(led_flag);
        }

        if (time_ms % 1 == 0)
        {
            adc_value_read(&process_handle, count_x, SINGLE_VCC_READ);

            ic_74hc595_set_io_scan(-1);

            if (count_x == 0)
                cal_resi_value(&process_handle, process_handle.x_max - 1);
            else
                cal_resi_value(&process_handle, count_x - 1);

            adc_value_read(&process_handle, count_x, SINGLE_RESI_READ);

            count_x ++;
            if (count_x == POS_X) count_x = 0;

            ic_74hc595_set_io_scan(count_x);
        }

        if(count_x == 0)
        {
            select_x_control(channel_count);

            {
                serial_frame.sof = 0x5aa5;
                serial_frame.tran_type = 0x01;
                serial_frame.len = sizeof(serial_frame)-2;
                serial_frame.type = 0x01;

                if (0)
                {
                    memset((uint8_t *)&serial_frame, 0x00, sizeof(SerialFrame_t));
                    printf_adc_value((uint8_t *)&serial_frame, sizeof(SerialFrame_t));
                    usb_send_buffer(usb_handle, (uint8_t *)&serial_frame, strlen((char *)(&serial_frame)));
                } else {
                    memcpy((uint8_t *)&serial_frame.adc_cali_value, process_handle.display_matrix, POS_X * POS_Y);
                    serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
                    usb_send_buffer(usb_handle, (uint8_t *)&serial_frame, sizeof(SerialFrame_t));
                }

                if(spi3_tx_start_flag == 0)
                {
                    LCD_ADC_DMA_display((uint8_t *)serial_frame.adc_cali_value);//函数执行时间2ms
                }
            }

        } else {
            //更改了延时中断时间，此时为 100us 而非 1ms
            delay_1ms(1);
        }
    }
}


//void get_mean_value(Struct_Adc *source_buffer, Struct_Adc*target_buffer,uint8_t mean_count)
//{
//
//		uint32_t sum = 0;
//		static uint32_t mean_loop = 0;
//		uint8_t min_value = 0xFF;
//		uint8_t max_value = 0;
//
//		if(mean_loop<mean_count-1)
//		{
//			mean_loop++;
//		}
//		else
//		{
//			mean_loop = 0;
//		}
//		if(mean_loop >= mean_count)
//		{
//			while(1);
//		}
//		memcpy(&mean_temp_buffer[mean_loop],source_buffer,POS_X*POS_Y);
//
//		for(uint8_t x = 0; x < POS_X; x++)
//		for(uint8_t y = 0; y < POS_Y; y++)
//		{
//			sum = 0;
//			min_value = 0xFF;
//			max_value = 0;
//			for(uint8_t i = 0; i < mean_count; i++)
//			{
//				if(mean_temp_buffer[i].adc_value[x][y] > max_value)
//				{
//					max_value = mean_temp_buffer[i].adc_value[x][y];
//				}
//				if(mean_temp_buffer[i].adc_value[x][y] < min_value)
//				{
//					min_value = mean_temp_buffer[i].adc_value[x][y];
//				}
//				sum += (uint32_t)mean_temp_buffer[i].adc_value[x][y];

//			}
//			target_buffer->adc_value[x][y]= (sum-(max_value+min_value))>>(mean_count-7);
//		}
//}

//void get_filter_value(Struct_Adc *source_buffer,Struct_Adc *target_buffer)
//{
//		uint8_t filter_value = 1;
//		for(uint8_t x = 0; x < POS_X; x++)
//		for(uint8_t y = 0; y < POS_Y; y++)
//		{
//			if(source_buffer->adc_value[x][y] < 2)
//			{
//				target_buffer->adc_value[x][y] =source_buffer->adc_value[x][y];
//			}
//			else if (abs(source_buffer->adc_value[x][y] - target_buffer->adc_value[x][y]) > filter_value)
//			{
//				target_buffer->adc_value[x][y] =((uint32_t)source_buffer->adc_value[x][y]*3 + (uint32_t)target_buffer->adc_value[x][y]*7)/10;
//			}
//
//			//filter_buffer.adc_value[x][y] = source_buffer->adc_value[x][y];
//		}
//}

uint16_t CalChecksum(uint8_t * data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}







