#include "led.h"
#include "spi.h"
#define SPI_CRC_ENABLE 1

uint8_t spi1_tx_start_flag = 0;


/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
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
//		//SPI1
//		gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
//    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
//    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
//		
//		//SPI3
//    gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
//    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
//    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);

//    /* set SPI3_NSS as GPIO*/
//    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
//    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	
		//SPI1
    gpio_af_set(GPIOB, GPIO_AF_5,  GPIO_PIN_13 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);

    /* set SPI3_NSS as GPIO*/
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

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

		spi_enable(SPI1);

}

static void dma_config()
{
	
    dma_single_data_parameter_struct dma_init_struct;
	

    /* configure SPI1 send dma: DMA_CH4 */
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
		dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;

    dma_single_data_mode_init(DMA0, DMA_CH4, &dma_init_struct);
    dma_channel_subperipheral_select(DMA0, DMA_CH4, DMA_SUBPERI0);
	
}




void SPI_init(void)
{
	rcu_config();
	gpio_config();
	spi_config();
	dma_config();
}



void spi1_send_dma_buffer(uint8_t *buffer, uint32_t len)
{
		//while(spi3_tx_start_flag == 1);
		SPI1_CS_GPIO_OUT(0);
		spi1_tx_start_flag = 1;
		
		spi_dma_disable(DMA0,DMA_CH4);
		dma_channel_disable(DMA0, DMA_CH4);
		dma_memory_address_config(DMA0, DMA_CH4, DMA_MEMORY_0,(uint32_t)buffer);
		dma_transfer_number_config(DMA0,DMA_CH4,len);
		spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
		dma_channel_enable(DMA0, DMA_CH4);
}



//·Åµ½SysTick_Handler(void)ÖÐ
void check_spi_flag(void)
{
		
		if(spi1_tx_start_flag == 1)
		{
				if(dma_flag_get(DMA0, DMA_CH4, DMA_FLAG_FTF))
 				if(spi_i2s_flag_get(SPI1,SPI_FLAG_TBE))
				if(!spi_i2s_flag_get(SPI1,SPI_FLAG_TRANS))
				{
						spi1_tx_start_flag = 0;
						SPI1_CS_GPIO_OUT(1);
						dma_flag_clear(DMA0, DMA_CH4, DMA_FLAG_FTF);
				}
		}
}
