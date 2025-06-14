#include "led.h"
#include "spi.h"
#define SPI_CRC_ENABLE 1

uint8_t spi1_rx_start_flag = 0;
uint8_t spi3_tx_start_flag = 0;

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rcu_config(void)
{
		rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOB);
		rcu_periph_clock_enable(RCU_DMA0);
		rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_SPI1);
		rcu_periph_clock_enable(RCU_SPI3);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gpio_config(void)
{
		//SPI1
		gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
		
		//SPI3
    gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14);

    /* set SPI1_NSS as GPIO*/
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

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
		spi_i2s_deinit(SPI3);
	
    spi_struct_para_init(&spi_init_struct);

    /* configure SPI0 parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_SLAVE;  
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_HARD;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);
		
		spi_init_struct.device_mode          = SPI_MASTER;
		spi_init_struct.nss                  = SPI_NSS_SOFT;
		spi_init(SPI3, &spi_init_struct);
	

#if SPI_CRC_ENABLE
    /* configure SPI CRC function */
    spi_crc_polynomial_set(SPI1, 7);
    spi_crc_on(SPI1);
#endif /* enable CRC function */
	

spi_enable(SPI1);
spi_enable(SPI3);

}

static void dma_config()
{
	
    dma_single_data_parameter_struct dma_init_struct;
	

    /* configure SPI1 send dma: DMA_CH4 */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.direction    = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
		dma_init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;

    dma_single_data_mode_init(DMA0, DMA_CH3, &dma_init_struct);
    dma_channel_subperipheral_select(DMA0, DMA_CH3, DMA_SUBPERI0);
		spi_dma_enable(SPI1, SPI_DMA_RECEIVE);
	
		dma_deinit(DMA1,DMA_CH4);
		dma_init_struct.periph_addr 	= (uint32_t)&SPI_DATA(SPI3);
		dma_init_struct.direction 		= DMA_MEMORY_TO_PERIPH;
		dma_single_data_mode_init(DMA1, DMA_CH4, &dma_init_struct);
		dma_channel_subperipheral_select(DMA1,DMA_CH4,DMA_SUBPERI5);
//		nvic_irq_enable(DMA1_Channel4_IRQn, 0, 0);
//		dma_interrupt_enable(DMA1, DMA_CH4, DMA_CHXCTL_FTFIE);
}




void SPI_init(void)
{
	rcu_config();
	gpio_config();
	spi_config();
	dma_config();
}

uint8_t  spi_rx_buffer(uint8_t *buffer,uint32_t len)
{
		uint8_t crc_value = 0;
		if(spi1_rx_start_flag == 0)
		{		
				spi_disable(SPI1);
				spi_i2s_data_receive(SPI1);//清除buffer
			
				dma_channel_disable(DMA0, DMA_CH3);
			
#if SPI_CRC_ENABLE
				if(SET != spi_i2s_flag_get(SPI1, SPI_FLAG_CRCERR))
				{
						crc_value = 1;
				}
				else
				{
						spi_crc_error_clear(SPI1);
				}
#endif	
				spi_crc_off(SPI1);
				
				dma_memory_address_config(DMA0, DMA_CH3, DMA_MEMORY_0,(uint32_t)buffer);
				dma_transfer_number_config(DMA0,DMA_CH3,len);
				
				while(gpio_input_bit_get(GPIOB, GPIO_PIN_12) == 0);
				spi_crc_on(SPI1);
				spi_crc_polynomial_set(SPI1, 7);
				spi_enable(SPI1);
				dma_channel_enable(DMA0, DMA_CH3);
				spi1_rx_start_flag = 1;
			}
	
	return crc_value;
}

void spi3_send_dma_buffer(uint8_t *buffer, uint32_t len)
{
		//while(spi3_tx_start_flag == 1);
		SPI3_CS_GPIO_OUT(0);
		spi3_tx_start_flag = 1;
		
		spi_dma_disable(DMA1,DMA_CH4);
		dma_channel_disable(DMA1, DMA_CH4);
		dma_memory_address_config(DMA1, DMA_CH4, DMA_MEMORY_0,(uint32_t)buffer);
		dma_transfer_number_config(DMA1,DMA_CH4,len);
		spi_dma_enable(SPI3, SPI_DMA_TRANSMIT);
		dma_channel_enable(DMA1, DMA_CH4);
}



//放到SysTick_Handler(void)中
void check_spi_flag(void)
{
		if(spi1_rx_start_flag == 1)
		{
				if(dma_flag_get(DMA0, DMA_CH3, DMA_FLAG_FTF))
				{
						dma_flag_clear(DMA0, DMA_CH3, DMA_FLAG_FTF);
						spi1_rx_start_flag = 0;
				}
		}
		
		if(spi3_tx_start_flag == 1)
		{
				if(dma_flag_get(DMA1, DMA_CH4, DMA_FLAG_FTF))
 				if(spi_i2s_flag_get(SPI3,SPI_FLAG_TBE))
				if(!spi_i2s_flag_get(SPI3,SPI_FLAG_TRANS))
				{
						spi3_tx_start_flag = 0;
						SPI3_CS_GPIO_OUT(1);
						dma_flag_clear(DMA1, DMA_CH4, DMA_FLAG_FTF);
				}
		}
}
