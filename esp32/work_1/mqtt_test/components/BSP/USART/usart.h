#ifndef _USART_H
#define _USART_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/uart_select.h"
#include "driver/gpio.h"


/* 引脚和串口定义 */
#define USART_UX            UART_NUM_0
#define USART_TX_GPIO_PIN   GPIO_NUM_43
#define USART_RX_GPIO_PIN   GPIO_NUM_44

/* 串口接收相关定义 */
#define RX_BUF_SIZE         1024    /* 环形缓冲区大小 */

/* 函数声明 */
void usart_init(uint32_t baudrate, uart_port_t uart_port);

#endif
