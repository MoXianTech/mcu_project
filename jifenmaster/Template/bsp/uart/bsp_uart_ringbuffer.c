#include "bsp_uart_ringbuffer.h"

circbuf_t com_circbuf[COM_UART_MAX_NUM] = {0};
int8_t uart_ringbuffer_init(uart_type_def uart_num, uint16_t uart_cache_size)
{
    return circbuf_alloc(com_circbuf + uart_num, uart_cache_size);
}

uint16_t uart_ringbuffer_pop_data(uart_type_def uart_num, uint8_t *data, uint16_t size)
{
    return circbuf_pop(com_circbuf + uart_num, data, size);
}

uint16_t uart_ringbuffer_push_data(uart_type_def uart_num, uint8_t *data, uint16_t size)
{
    return circbuf_push(com_circbuf + uart_num, data, size);
}

uint16_t uart_ringbuffer_getused(uart_type_def uart_num)
{
    return  circbuf_getusedsize(com_circbuf + uart_num);
}

void uart_ringbuffer_deinit(uart_type_def uart_num)
{
    circbuf_free(com_circbuf + uart_num);
}
