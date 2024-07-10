#ifndef __BSP_UART_RINGBUFFER_H__
#define __BSP_UART_RINGBUFFER_H__
#include "circular_buffer.h"
#include "priv_malloc.h"
#include "bsp_uart.h"


int8_t uart_ringbuffer_init(uart_type_def uart_num, uint16_t uart_cache_size);
uint16_t uart_ringbuffer_pop_data(uart_type_def uart_num, uint8_t *data, uint16_t size);
uint16_t uart_ringbuffer_push_data(uart_type_def uart_num, uint8_t *data, uint16_t size);
uint16_t uart_ringbuffer_getused(uart_type_def uart_num);
void uart_ringbuffer_deinit(uart_type_def uart_num);
uint16_t uart_ringbuffer_read_data(uart_type_def uart_num, uint8_t *data, uint16_t size);
void uart_ringbuffer_drop_data(uart_type_def uart_num, uint16_t lentodrop);

#endif
