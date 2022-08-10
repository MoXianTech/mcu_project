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

typedef struct 
{
  uint16_t sof;
	uint8_t tran_type;
	uint16_t len;
	uint8_t type;
  uint8_t adc_value[16*16];
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
	RED_LED(1);
	while( 1 )
	{
		time_1ms ++;
		//LED闪烁
		if(time_1ms % 500 == 0)
		{
			static bool led_flag = false;
			led_flag = !led_flag;
			//RED_LED(led_flag);
      GREEN_LED(!led_flag);
		}
		//设置分压电阻
		if(time_1ms % 500 == 0)
		{
			//ChooseEnlarge(GetSwStatus());
			ChooseEnlarge(3 );
		}
		
		//50Hz速率采集第一快压感数据
		if(time_1ms % 100 == 0)
		{
			temp_adc_value = ReadAdcValue();
			//数据填充到C128R16格式
			for(int j = 0; j < 16; ++j) 
			{
				for(int i = 0; i < 16; ++i)
				{
					c128_r16_value.adc_value[0][j][i] = temp_adc_value.value[j][i];
				}
			}
		}
		
		//大约25Hz上传所有的数据
		if(time_1ms % 100 == 0)
		{
			//数据排列转换
			C128R16ToC64R32();
			
			serial_frame.sof = 0x5aa5;
			serial_frame.tran_type = 0x01;
			serial_frame.len = sizeof(serial_frame) - 2;
			serial_frame.type = 0x01;
			for(int j = 0; j < 16; ++j)
			{
				for(int i = 0; i < 16; ++i)
				{
					serial_frame.adc_value[j * 16 + i] = temp_adc_value.value[j][i];
				}
			}
			serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
			dma_send_data((uint8_t *)&serial_frame, sizeof(serial_frame));
		}
		delay_1ms(1);
	}
}

//由接收到的C128R16格式的数据转换成C64R32格式的数据
void C128R16ToC64R32(void)
{
	for(int C = 0; C < 16; ++C)
	{
		for(int R = 0; R < 32; ++R)
		{
			if(R < 16)
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[4][C][15 - R];
			}
			else
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[3][C][R - 16];
			}
		}
	}
	
	for(int C = 16; C < 32; ++C)
	{
		for(int R = 0; R < 32; ++R)
		{
			if(R < 16)
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[5][C - 16][15 - R];
			}
			else
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[2][C - 16][R - 16];
			}
		}
	}
	
	for(int C = 32; C < 48; ++C)
	{
		for(int R = 0; R < 32; ++R)
		{
			if(R < 16)
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[6][C - 32][15 - R];
			}
			else
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[1][C - 32][R - 16];
			}
		}
	}
	
	for(int C = 48; C < 64; ++C)
	{
		for(int R = 0; R < 32; ++R)
		{
			if(R < 16)
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[7][C - 48][15 - R];
			}
			else
			{
				c64_r32_value.adc_value[C][R] = c128_r16_value.adc_value[0][C - 48][R - 16];
			}
		}
	}
}

can_receive_message_struct main_g_receive_message;
void read_can0_data(can_receive_message_struct *receive_message)
{
    main_g_receive_message = *receive_message;
	//0x200~ 0x700 第2块·到第7块
	//接收2到7块的数据
	//C1R1~C1R8 0x101
	//C1R9~C1R16 0x102
	//......
	//C16R1~C1R8 0x131
	//C16R9~C1R16 0x132
	for(uint32_t id_head = 2; id_head < 8; ++id_head)
	{
		for(uint32_t id_end = 0; id_end < 32; ++id_end)
		{
			if(main_g_receive_message.rx_sfid == 0x100 * id_head + id_end)
			{
				for(int i = 0; i < 8; ++i)
				{
					if(id_end % 2 == 0)
					{
						c128_r16_value.adc_value[id_head - 1][id_end / 2][i] = main_g_receive_message.rx_data[i];
					}
					if(id_end % 2 == 1)
					{
						c128_r16_value.adc_value[id_head - 1][id_end / 2][i + 8] = main_g_receive_message.rx_data[i];
					}
				}
			}
		}
	}
	
	//0x100 设置为第8块的ID，为什么不设置为0x800，由于设置为0x800后，ID会被识别为0x000，未排除原因，所以先暂时把第8块ID设置为0x100
	//接收8的数据
	//C1R1~C1R8 0x101
	//C1R9~C1R16 0x102
	//......
	//C16R1~C1R8 0x131
	//C16R9~C1R16 0x132
	for(uint32_t id_end = 0; id_end < 32; ++id_end)
	{
		if(main_g_receive_message.rx_sfid == 0x100 * 1 + id_end)
		{
			for(int i = 0; i < 8; ++i)
			{
				if(id_end % 2 == 0)
				{
					c128_r16_value.adc_value[7][id_end / 2][i] = main_g_receive_message.rx_data[i];
				}
				if(id_end % 2 == 1)
				{
					c128_r16_value.adc_value[7][id_end / 2][i + 8] = main_g_receive_message.rx_data[i];
				}
			}
		}
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
    can_receive_message_struct receive_message;
    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
    read_can0_data(&receive_message);
			static bool led_flag = false;
			led_flag = !led_flag;
			GREEN_LED(led_flag);
}

void TIMER3_IRQHandler( void )
{
	if( SET == timer_interrupt_flag_get( TIMER3, TIMER_INT_FLAG_UP ) )
	{
		/* clear channel 0 interrupt bit */
		timer_interrupt_flag_clear( TIMER3, TIMER_INT_FLAG_UP );
	
	}
}
