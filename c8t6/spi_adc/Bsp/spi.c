#include "spi.h"

uint8_t spi1_send_array[ARRAYSIZE] = {0};

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
		rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_SPI1);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gpio_config(void)
{
    /* configure SPI1 GPIO: SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI1);
    spi_struct_para_init(&spi_init_struct);

    /* configure SPI0 parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;  
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

#if SPI_CRC_ENABLE
    /* configure SPI CRC function */
    spi_crc_polynomial_set(SPI0, 7);
    spi_crc_polynomial_set(SPI2, 7);
    spi_crc_on(SPI0);
    spi_crc_on(SPI2);
#endif /* enable CRC function */

spi_enable(SPI1);
//spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);

}

static void dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    dma_struct_para_init(&dma_init_struct);

    /* configure SPI1 send dma: DMA_CH4 */
    dma_deinit(DMA0, DMA_CH4);

    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.memory_addr  = (uint32_t)spi1_send_array;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = ARRAYSIZE;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);
    dma_memory_to_memory_disable(DMA0, DMA_CH4);
		spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
	
}


void SPI_init(void)
{
	rcu_config();
	gpio_config();
	spi_config();
}

void mydma_enable(void)
{
		dma_channel_disable(DMA0, DMA_CH4);
		dma_transfer_number_config(DMA0,DMA_CH4,ARRAYSIZE);
		dma_channel_enable(DMA0, DMA_CH4);
}

void SPI_DMA_init(void)
{
	dma_config();
	mydma_enable();
}

void delay_us(uint8_t xus)
{
	while(xus--);
}
