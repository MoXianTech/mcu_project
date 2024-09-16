#ifndef _SPI_H
#define _SPI_H
#include "gd32e10x.h"

#define ARRAYSIZE 1296

extern uint8_t spi1_send_array[ARRAYSIZE];


void SPI_init(void);
void delay_us(uint8_t xus);
void mydma_enable(void);
void SPI_DMA_init(void);

#endif
