#include "bsp_switch.h"
#include "board.h"
#include "PID_moto_control.h"
#include "bsp_encoder.h"


uint16_t adc_value[5] = {0};

void switch_gpio_init(void)
{
    //使能引脚时钟
    rcu_periph_clock_enable(RCU_SWITCH);
    //配置引脚为模拟浮空输入模式
    gpio_mode_set(PORT_SWITCH0, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_SWITCH0);
    gpio_mode_set(PORT_SWITCH1, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_SWITCH1);
    gpio_mode_set(PORT_SWITCH2, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_SWITCH2);
    gpio_mode_set(PORT_SWITCH3, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_SWITCH3);
    gpio_mode_set(PORT_SWITCH4, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_SWITCH4);
}

void switch_adc_init(void)
{

    //使能ADC时钟
    rcu_periph_clock_enable(SWITCH_ADC_RCU);
    //配置ADC时钟
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);

    //使能扫描模式
    adc_special_function_config(SWITCH_ADC, ADC_SCAN_MODE, DISABLE);

    //配置ADC为独立模式      
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);

    //数据右对齐     
    adc_data_alignment_config(SWITCH_ADC, ADC_DATAALIGN_RIGHT);

    //ADC0设置为12位分辨率               
    adc_resolution_config(SWITCH_ADC, ADC_RESOLUTION_12B);

    //ADC0设置为规则组  一共使用 1 个通道
    adc_channel_length_config(SWITCH_ADC,ADC_ROUTINE_CHANNEL, 1);

    //ADC0使能
    adc_enable(SWITCH_ADC);

    //ADC外部触发禁用, 即只能使用软件触发
    adc_external_trigger_config(SWITCH_ADC,ADC_ROUTINE_CHANNEL,EXTERNAL_TRIGGER_DISABLE);

    //使能软件触发
    //adc_software_trigger_enable(SWITCH_ADC, ADC_ROUTINE_CHANNEL);

    //开启ADC自校准
    adc_calibration_enable(SWITCH_ADC);
}

/**********************************************************
 * 函 数 名 称：Get_ADC_Value
 * 函 数 功 能：读取ADC值
 * 传 入 参 数：sitch_num = 要采集的通道
 * 函 数 返 回：测量到的值
 * 作       者：LC
 * 备       注：默认采样周期为15个ADC采样时间
 **********************************************************/
uint16_t get_switch_value(SWITCH_NUM num)
{
#ifndef ENABLE_SWITCH_DMA_ADC
    unsigned int adc_value = 0;
    //设置采集通道
    adc_routine_channel_config(SWITCH_ADC, 0, num, ADC_SAMPLETIME_15);
    //开始软件转换
    adc_software_trigger_enable(SWITCH_ADC, ADC_ROUTINE_CHANNEL);
    //等待 ADC0 采样完成 
    while(adc_flag_get(SWITCH_ADC, ADC_FLAG_EOC) == RESET) 
    {}
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    //读取采样值
    adc_value = adc_routine_data_read(SWITCH_ADC);
    //返回采样值
    return adc_value;
#else
    return adc_value[num - SWITCH_0];
#endif
}

/*!
  \brief      configure the different system clocks
  \param[in]  none
  \param[out] none
  \retval     none
  */
void switch_dma_rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
}

/*!
  \brief      configure the DMA peripheral
  \param[in]  none
  \param[out] none
  \retval     none
  */
void switch_dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA1, DMA_CH0);

    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(adc_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = 5;
    dma_single_data_parameter.priority = DMA_PRIORITY_MEDIUM;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_single_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);

    /* enable DMA circulation mode */
    dma_circulation_enable(DMA1, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
}

/*!
  \brief      configure the ADC peripheral
  \param[in]  none
  \param[out] none
  \retval     none
  */
void switch_dma_adc_config(void)
{
    /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC contineous function disable */
    adc_special_function_config(SWITCH_ADC, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan mode disable */
    adc_special_function_config(SWITCH_ADC, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(SWITCH_ADC, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(SWITCH_ADC, ADC_ROUTINE_CHANNEL, 5);

    /* ADC routine channel config */
    adc_routine_channel_config(SWITCH_ADC, 0, SWITCH0_ADC_CHANNEL, ADC_SAMPLETIME_15);
    adc_routine_channel_config(SWITCH_ADC, 1, SWITCH1_ADC_CHANNEL, ADC_SAMPLETIME_15);
    adc_routine_channel_config(SWITCH_ADC, 2, SWITCH2_ADC_CHANNEL, ADC_SAMPLETIME_15);
    adc_routine_channel_config(SWITCH_ADC, 3, SWITCH3_ADC_CHANNEL, ADC_SAMPLETIME_15);
    adc_routine_channel_config(SWITCH_ADC, 4, SWITCH4_ADC_CHANNEL, ADC_SAMPLETIME_15);

    /* ADC trigger config */
    adc_external_trigger_source_config(SWITCH_ADC, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0); 
    adc_external_trigger_config(SWITCH_ADC, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(SWITCH_ADC);
    adc_dma_mode_enable(SWITCH_ADC);

    /* enable ADC interface */
    adc_enable(SWITCH_ADC);
    /* wait for ADC stability */
    delay_ms(10);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(SWITCH_ADC);

    /* enable ADC software trigger */
    adc_software_trigger_enable(SWITCH_ADC, ADC_ROUTINE_CHANNEL);
}

void switch_dma_adc_init(void)
{
    switch_dma_rcu_config();
    switch_dma_config();
    switch_dma_adc_config();
}

void switch_bsp_init(void)
{
    switch_gpio_init();
#ifndef ENABLE_SWITCH_DMA_ADC
    switch_adc_init();
#else
    switch_dma_adc_init();
#endif
}

void switch_thread_init()
{
    switch_bsp_init();
}

void switch_thread_run()
{

#ifndef ENABLE_SWITCH_DMA_ADC
    adc_value[0] = get_switch_value(SWITCH_0);
    adc_value[1] = get_switch_value(SWITCH_1);
    adc_value[2] = get_switch_value(SWITCH_2);
    adc_value[3] = get_switch_value(SWITCH_3);
    adc_value[4] = get_switch_value(SWITCH_4);

#else
    uint16_t moto2_switch = 0, moto4_switch = 0;

    int16_t count = get_encoder_currentCount(MOTO_B);
    moto2_switch = get_switch_value(SWITCH_1);
    if (moto2_switch > 2500)
    {
        set_moto_target(MOTO_2, 100);
        printf("%d\n", count);
    }
    else if (moto2_switch > 1900)
    {
        set_moto_target(MOTO_2, -100);
        printf("%d\n", count);
    }
    else {
        set_moto_stop(MOTO_2);
    }

    moto4_switch = get_switch_value(SWITCH_0);
    if (moto4_switch > 2500)
    {
        set_moto_target(MOTO_4, 100);
    }
    else if (moto4_switch > 1900)
    {
        set_moto_target(MOTO_4, -100);
    }
    else {
        set_moto_stop(MOTO_4);
    }
#endif


#if 0

    printf("software adc %d %d %d %d %d\n",
            adc_value[SWITCH_0 - SWITCH_0],
            adc_value[SWITCH_1 - SWITCH_0],
            adc_value[SWITCH_2 - SWITCH_0],
            adc_value[SWITCH_3 - SWITCH_0],
            adc_value[SWITCH_4 - SWITCH_0]);
#endif

}
