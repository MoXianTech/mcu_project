/**
* @file                hal_uart.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup HAL层
  * @{
  */
/**
 * @defgroup UART驱动
 * @brief UART驱动
 * @details 
 * @{
 */

//driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_syscfg.h"
#include "csa37f7x_usart.h"
#include "csa37f7x_gpio.h"
//hal
#include "hal_uart.h"
/**
  * @fn void hal_uart_init(void)
  * @brief  Uart init
  * @param None
  * @return None
 */
void hal_uart_init(void)                                        // 串口初始化
{
    usart_config_t r_usart_config;

    /*使能串口时钟*/
    rcc_apb_periph_clock_enable_ctrl(RCC_APBPeriph_USART,ENABLE);

    /*波特率选择 */
    r_usart_config.usart_rate = 115200;
    /*数据长度 */
    r_usart_config.data_width = USART_DATA_WIDTH_8;
    /*校验使能 */
    r_usart_config.usart_parity = DISABLE;
    /*停止位 */
    r_usart_config.stop_bits = USART_STOP_BIT_1;
    /*选择收发模式*/
    r_usart_config.usart_mode = USART_MODE_RX|USART_MODE_TX;

    /*串口初始化 */
    usart_init(&r_usart_config);

    /*复用PA11为RX */
    gpio_mf_config(GPIO_GROUP_A,GPIO_PIN_NUM11,GPIO_MUX_FUNC_3);
    /*复用PA10为TX */
    gpio_mf_config(GPIO_GROUP_A,GPIO_PIN_NUM10,GPIO_MUX_FUNC_3);

    usart_direction_enable_ctrl(USART_MODE_RX|USART_MODE_TX,ENABLE);
    usart_enable_ctrl(ENABLE);
}

/**
  * @fn void hal_uart_send_byte(unsigned char dat)
  * @brief uart send data
  * @param [in]dat: data to send
  * @return None
 */
void hal_uart_send_word_ascii(uint16_t dat)                      // 串口发送数据函数
{
    uint8_t i;
    uint8_t uart_temp;
    
    while(usart_flag_status_get(USART_FLAG_TCF) == 0){
    }
    i = 16;
    do{
        i -= 4;
        uart_temp = (dat >> i) & 0x0f;
        if(uart_temp > 9){
            uart_temp = 'A' + uart_temp - 0x0A;
        }
        else{
            uart_temp += '0';
        }
        hal_uart_send_byte(uart_temp);
    }
    while(i > 0);
    hal_uart_send_byte(',');
}


/**
  * @fn void hal_uart_send_byte(unsigned char dat)
  * @brief uart send data
  * @param [in]dat: data to send
  * @return None
 */
void hal_uart_send_byte(unsigned char dat)                      // 串口发送数据函数
{
    usart_data_send(dat);
    while(usart_flag_status_get(USART_FLAG_TCF) == 0)
    {
    }
}

/**
  * @}
  */

/**
  * @}
  */
