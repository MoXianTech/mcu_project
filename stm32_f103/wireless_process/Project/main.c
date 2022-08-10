//=============================================================================
//文件名称:main.c
//功能概要:延时闪烁PC13连接的LED
//         低电平LED——>亮，高电平-->灭
//版权所有:源地工作室 http://www.vcc-gnd.com/  网店 http://vcc-gnd.taobao.com/
//版本更新:2013-10-09 v1.0
//调试方式:ST-Link
//=============================================================================


//头文件
#include "stm32f10x.h"
#include "usart.h"
#include "led.h"
#include "RTC_Time.h"
#include "nrf24l01.h"
#include "io_api.h"
#include <stdio.h>

//=============================================================================
//函数名称:Delay
//功能概要:延时函数
//参数说明:无
//函数返回:无
//=============================================================================
void  Delay (volatile uint64_t nCount)
{
    volatile int64_t count;
    for(; nCount != 0; nCount--)
    {
        for(count = 250000; count <= 0; count --);
    }
}

#define USART_REC_LEN 31

typedef struct usart_context {
    uint8_t usart_buffer[USART_REC_LEN];
    uint8_t buffer_end;
    uint8_t buffer_len;
    bool cache_enable;
} usart_context_t;

usart_context_t usart_context = {0};
usart_context_t usart_context_1 = {0};

uint8_t cache_buffer[32] = {0x11,0x22,0x33};

uint16_t USART_RX_STA = 0;
uint8_t cache_value = 0;

int main(void)
{
    bool led_flag = 0;

    LED_GPIO_Config();//初始化LED
    car_driver_io_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    USART1_Config(460800);
    nrf24l01_init();

    printf("*****************************************************************\r\n");
    while (nrf24l01_check())
    {
        printf("        nrf24l01 init success, 奥里给!!!!\n");
    }
    printf("*****************************************************************\r\n");

    nrf24l01_rt_init();
    //RTC_Init();

    while (1)
    {
        if (led_flag)
            LED0_ON;
        else
            LED0_OFF;
        led_flag = !led_flag;

        if (PAin(2) == 0)
        {
            nrf24l01_rx_packet(cache_buffer);
            printf("recive !!! %x  \n", cache_buffer[1]);

            switch (cache_buffer[1])
            {
                case 0x31:
                    PBout(0) = 1;
                    PBout(1) = 0;

                    PBout(10) = 1;
                    PBout(11) = 0;
                break;

                case 0x32:
                    PBout(0) = 0;
                    PBout(1) = 1;
 
                    PBout(10) = 0;
                    PBout(11) = 1;
                break;

                case 0x33:
                    PBout(0) = 1;
                    PBout(1) = 0;
 
                    PBout(10) = 0;
                    PBout(11) = 1;
                break;

                case 0x34:
                    PBout(0) = 0;
                    PBout(1) = 1;
 
                    PBout(10) = 1;
                    PBout(11) = 0;
                break;

                default:
                    PBout(0) = 0;
                    PBout(1) = 0;
  
                    PBout(10) = 0;
                    PBout(11) = 0;
                break;
            }

        }
 
//        nrf24l01_send_buf(cache_buffer);
        Delay(50000);
    }
}

void USART1_IRQHandler()
{
    if (RESET != USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        cache_value = USART_ReceiveData(USART1);

        {
            usart_context.usart_buffer[usart_context.buffer_end] = cache_value;
            usart_context.buffer_end ++;
            if (usart_context.buffer_end >= usart_context.buffer_len)
            {
                usart_context.buffer_end = 0;
                memcpy(usart_context_1.usart_buffer, usart_context.usart_buffer, usart_context.buffer_len);
                usart_context_1.buffer_end = usart_context.buffer_len;
            }
        }
    }
}
/*********************************************************************************************************
  END FILE
 *********************************************************************************************************/





