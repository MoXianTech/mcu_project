#ifndef _SPI_H
#define _SPI_H
#include "gd32f4xx.h"

//#define ARRAYSIZE 3

#define SPI3_CS_GPIO_OUT(a)	if (a)	\
    gpio_bit_set(GPIOE, GPIO_PIN_11);\
    else		\
    gpio_bit_reset(GPIOE, GPIO_PIN_11)	

extern uint8_t spi3_tx_start_flag;
extern uint8_t spi1_rx_start_flag;

void SPI_init(void);
void SPI_DMA_init(void);
uint8_t  spi_rx_buffer(uint8_t *buffer,uint32_t len);
void spi3_send_dma_buffer(uint8_t *buffer, uint32_t len);
void check_spi_flag(void);
		
#endif
