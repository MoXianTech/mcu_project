/**
* @file                hal_uart.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>                   /* Include standard types */

void hal_uart_init(void);
void hal_uart_send_byte(uint8_t dat);
void hal_uart_send_word_ascii(uint16_t dat) ;
uint8_t hal_uart_send_state(void);

#endif /* HAL_UART_H */
