#include "adc.h"


#define ADC_COUNT_NUM 1
#define ADC_ENABLE_NUM 10
volatile uint16_t adc_value[ADC_COUNT_NUM][ADC_ENABLE_NUM];

uint16_t mean_filter(volatile uint16_t (*arr)[10], uint8_t index)
{
    uint16_t maxIndex = 0;
    uint16_t minIndex = 0;
    uint8_t count = 0;
    uint32_t sum = 0;

    for (; count < ADC_COUNT_NUM; count ++)
    {
        if (arr[count][index] > arr[maxIndex][index])
        {
            maxIndex = count;
        }
        if (arr[count][index] < arr[minIndex][index])
        {
            minIndex = count;
        }
    }
    for (count = 0; count < ADC_COUNT_NUM; count ++)
    {
        if (count != maxIndex && count != minIndex)
        {
            sum += arr[count][index];
        }
    }
    return sum / ADC_COUNT_NUM;
}

uint16_t sum_filter(volatile uint16_t (*arr)[10], uint16_t index)
{
    uint8_t count = 0;
    uint32_t sum_value = 0;

    for (count = 0; count < ADC_COUNT_NUM; count ++)
    {
        sum_value += arr[count][index];
    }

    return sum_value / (ADC_COUNT_NUM);
}

uint16_t GetAdcValue(uint8_t index)
{
    if(index < ADC_ENABLE_NUM)
    {
        return adc_value[0][index];
    }
    else
    {
        return 0;
    }
}

void adc_init(void)
{
    /* system clocks configuration */
    rcu_config();
    /* systick configuration */
    systick_config();
    /* GPIO configuration */
    gpio_config();
    /* DMA configuration */
    dma_config();
    /* ADC configuration */
    adc_config();
}

/*!
  \brief      configure the different system clocks
  \param[in]  none
  \param[out] none
  \retval     none
  */
void rcu_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* config ADC clock */
    //rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
}

/*!
  \brief      configure the GPIO peripheral
  \param[in]  none
  \param[out] none
  \retval     none
  */
void gpio_config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOA,GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1 |
                                                          GPIO_PIN_2 |
                                                          GPIO_PIN_3 |
                                                          GPIO_PIN_4 |
                                                          GPIO_PIN_5 |
                                                          GPIO_PIN_6 |
                                                          GPIO_PIN_7
                                                          );

    gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);

    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG,  GPIO_PUPD_NONE, GPIO_PIN_4 |
                                                            GPIO_PIN_5);
}

/*!
  \brief      configure the DMA peripheral
  \param[in]  none
  \param[out] none
  \retval     none
  */
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA1, DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory0_addr = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    //dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_data_parameter.number = ADC_COUNT_NUM * ADC_ENABLE_NUM;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);

    dma_circulation_enable(DMA1, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
    //dma interrupt
    //dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
}

void DMA1_Channel0_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA1, DMA_CH0, DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA1, DMA_CH0, DMA_INT_FLAG_FTF);
    }
}


/*!
  \brief      configure the ADC peripheral
  \param[in]  none
  \param[out] none
  \retval     none
  */
void adc_config(void)
{

    /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC contineous function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_ENABLE_NUM);
    /* ADC regular channel config */

    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_1, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_2, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 2, ADC_CHANNEL_3, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 3, ADC_CHANNEL_4, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 4, ADC_CHANNEL_5, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 5, ADC_CHANNEL_6, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 6, ADC_CHANNEL_7, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 7, ADC_CHANNEL_14, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 8, ADC_CHANNEL_15, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 9, ADC_CHANNEL_8, ADC_SAMPLETIME_15);

    /*
    adc_routine_channel_config(ADC0, 10, ADC_CHANNEL_11, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 11, ADC_CHANNEL_12, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 12, ADC_CHANNEL_13, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 13, ADC_CHANNEL_14, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 14, ADC_CHANNEL_15, ADC_SAMPLETIME_15);
    */

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(ADC0);
    adc_dma_mode_enable(ADC0);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(100);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    delay_1ms(100);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}

void printf_adc_value(uint8_t *printf_buffer, uint16_t buffer_size)
{
    sprintf((char *)printf_buffer, "adc_value = %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
            , adc_value[0][0]
            , adc_value[0][1]
            , adc_value[0][2]
            , adc_value[0][3]
            , adc_value[0][4]
            , adc_value[0][5]
            , adc_value[0][6]
            , adc_value[0][7]
            , adc_value[0][8]
            , adc_value[0][9]
           );
}
