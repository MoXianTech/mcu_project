#ifndef _SPI_H
#define _SPI_H
#include "gd32f4xx.h"

//#define ARRAYSIZE 3

#define SPI1_CS_GPIO_OUT(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_12);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_12)	

extern uint8_t spi1_tx_start_flag;


void SPI_init(void);
void SPI_DMA_init(void);
void spi1_send_dma_buffer(uint8_t *buffer, uint32_t len);
void check_spi_flag(void);
		
#endif
