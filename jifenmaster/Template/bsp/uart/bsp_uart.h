/*
* 梁山派软硬件资料与相关扩展板软硬件资料官网全部开源
* 开发板官网：www.lckfb.com
* 技术支持常驻论坛，任何技术问题欢迎随时交流学习
* 立创论坛：club.szlcsc.com
* 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
* 不靠卖板赚钱，以培养中国工程师为己任
* Change Logs:
* Date           Author       Notes
* 2023-11-02     LCKFB-yzh    first version
*/

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "board.h"

//基本不使用USART，平时用的都是UART
typedef enum
{
    COM_UART_NUM0 = 0,
    COM_UART_NUM1 = 1,
    COM_UART_NUM2 = 2,
    COM_UART_NUM5 = 3,
    COM_UART_MAX_NUM
} uart_type_def;

#define COM_UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define COM_UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

//串口0的宏定义
#define COM_UART0                         USART0
#define COM_UART0_IRQn                    USART0_IRQn
#define COM_UART0_CLK                     RCU_USART0
#define COM_UART0_TX_GPIO_CLK             RCU_GPIOA
#define COM_UART0_TX_GPIO_AF              GPIO_AF_7
#define COM_UART0_TX_PORT                 GPIOA
#define COM_UART0_TX_PIN                  GPIO_PIN_9
#define COM_UART0_RX_GPIO_CLK             RCU_GPIOA
#define COM_UART0_RX_GPIO_AF              GPIO_AF_7
#define COM_UART0_RX_PORT                 GPIOA
#define COM_UART0_RX_PIN                  GPIO_PIN_10
#define COM_UART0_BAUD                    115200U

//串口1的宏定义
#define COM_UART1                         USART1
#define COM_UART1_IRQn                    USART1_IRQn
#define COM_UART1_CLK                     RCU_USART1
#define COM_UART1_TX_GPIO_CLK             RCU_GPIOA
#define COM_UART1_TX_GPIO_AF              GPIO_AF_7
#define COM_UART1_TX_PORT                 GPIOD
#define COM_UART1_TX_PIN                  GPIO_PIN_5
#define COM_UART1_RX_GPIO_CLK             RCU_GPIOD
#define COM_UART1_RX_GPIO_AF              GPIO_AF_7
#define COM_UART1_RX_PORT                 GPIOD
#define COM_UART1_RX_PIN                  GPIO_PIN_6
#define COM_UART1_BAUD                    115200U

//串口2的宏定义
#define COM_USART2                         USART2
#define COM_USART2_IRQn                    USART2_IRQn
#define COM_USART2_CLK                     RCU_USART2
#define COM_USART2_TX_GPIO_CLK             RCU_GPIOC
#define COM_USART2_TX_GPIO_AF              GPIO_AF_7
#define COM_USART2_TX_PORT                 GPIOC
#define COM_USART2_TX_PIN                  GPIO_PIN_11
#define COM_USART2_RX_GPIO_CLK             RCU_GPIOC
#define COM_USART2_RX_GPIO_AF              GPIO_AF_7
#define COM_USART2_RX_PORT                 GPIOC
#define COM_USART2_RX_PIN                  GPIO_PIN_10
#define COM_USART2_BAUD                    460800U
#define COM_USART2_DMA_RCU                 RCU_DMA0
#define COM_USART2_DMA                     DMA0
#define COM_USART2_TX_DMA_SUBPERI          DMA_SUBPERI4
#define COM_USART2_RX_DMA_SUBPERI          DMA_SUBPERI4
#define COM_USART2_TX_DMA_CHANNEL          DMA_CH3
#define COM_USART2_RX_DMA_CHANNEL          DMA_CH1
#define COM_USART2_RX_DMA_IRQ              DMA0_Channel1_IRQn


//串口5的宏定义
#define COM_USART5                         USART5
#define COM_USART5_IRQn                    USART5_IRQn
#define COM_USART5_CLK                     RCU_USART5
#define COM_USART5_TX_GPIO_CLK             RCU_GPIOC
#define COM_USART5_TX_GPIO_AF              GPIO_AF_8
#define COM_USART5_TX_PORT                 GPIOC
#define COM_USART5_TX_PIN                  GPIO_PIN_6
#define COM_USART5_RX_GPIO_CLK             RCU_GPIOC
#define COM_USART5_RX_GPIO_AF              GPIO_AF_8
#define COM_USART5_RX_PORT                 GPIOC
#define COM_USART5_RX_PIN                  GPIO_PIN_7
#define COM_USART5_BAUD                    460800U
#define COM_USART5_DMA_RCU                 RCU_DMA1
#define COM_USART5_DMA                     DMA1
#define COM_USART5_TX_DMA_SUBPERI          DMA_SUBPERI5
#define COM_USART5_RX_DMA_SUBPERI          DMA_SUBPERI5
#define COM_USART5_TX_DMA_CHANNEL          DMA_CH7
#define COM_USART5_RX_DMA_CHANNEL          DMA_CH2
#define COM_USART5_RX_DMA_IRQ              DMA1_Channel2_IRQn

#define ENABLE_USART_DMA

void bsp_uart_init(void);
void bsp_uart_periph_init(uart_type_def _com_uart);

void usart_thread_init(void);
void usart_thread_run(void);
void usart_dma_test(uart_type_def uart_num);

#endif /* __BSP_UART_H__ */
