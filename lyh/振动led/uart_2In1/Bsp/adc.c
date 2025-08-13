#include "adc.h"
#include <stdio.h>

#include "main.h"
//#include "74hc4051bq.h"


//volatile uint16_t adc_value1[8];
//uint16_t  adc_v[adc_channel_num] = {0};
//uint16_t  max_value111;
//void read_adc_value(void)
//{
////  uint16_t sum = 0;
////	
////	for(uint8_t adc_channel_index = 0; adc_channel_index < adc_channel_num; adc_channel_index++){
////      sum = 0;
////			for(uint8_t cumulative_index = 0; cumulative_index < cumulative_num; cumulative_index++)
////       {
////				 sum += adc_value1[cumulative_index];
////			 }
////			adc_v[adc_channel_index] = sum/cumulative_num;
////	}
////	for (uint8_t i = 0; i < 8; i++) {
////        if (adc_v[i] > max_value111) {
////            max_value111 = adc_v[i]; // 更新最大值
////        }
////    }
//}

//uint16_t GetAdcValue(uint8_t index)
//{
//		read_adc_value();
//    if(index < 8)
//    {
//        return adc_v[index];
//    }
//    else
//    {
//        return 0;
//    }
//}

//void adc_init(void)
//{
//    /* system clocks configuration */
//    rcu_config();
//    /* systick configuration */
//    systick_config();
//    /* GPIO configuration */
//    gpio_config();
//    /* DMA configuration */
//    dma_config();
//    /* ADC configuration */
//    adc_config();
//}

///*!
//    \brief      configure the different system clocks
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
void rcu_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
}

///*!
//    \brief      configure the GPIO peripheral
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void gpio_config(void)
//{
//    /* config the GPIO as analog mode */
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);  
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);
//    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2);
//    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3);
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_4);  
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_5);
//    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_6);
//    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_7);
//}


///*!
//    \brief      configure the DMA peripheral
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void dma_config(void)
//{
//    /* ADC_DMA_channel configuration */
//    dma_parameter_struct dma_data_parameter;

//    /* ADC DMA_channel configuration */
//    dma_deinit(DMA0, DMA_CH0);

//    /* initialize DMA single data mode */
//    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
//    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
//    dma_data_parameter.memory_addr = (uint32_t)(&adc_value1);
//    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
//    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
//    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
//    dma_data_parameter.number = cumulative_num * adc_channel_num;
//    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
//    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

//    dma_circulation_enable(DMA0, DMA_CH0);

//    /* enable DMA channel */
//    dma_channel_enable(DMA0, DMA_CH0);
//    //dma interrupt
//    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
//}

//void DMA0_Channel0_IRQHandler(void)
//{
//    if(dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF))
//    {
//        dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_G);
////        read_adc_value();
//    }
//}


///*!
//    \brief      configure the ADC peripheral
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void adc_config(void)
//{
//    /* reset ADC */
//    adc_deinit(ADC0);
//    /* ADC mode config */
//    adc_mode_config(ADC_MODE_FREE);
//    /* ADC contineous function enable */
//    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
//    /* ADC scan mode disable */
//    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
//    /* ADC data alignment config */
//    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
//	
//	  
//		/* 16 times sample, 4 bits shift */
////    adc_oversample_mode_config(ADC0, ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_4B, ADC_OVERSAMPLING_RATIO_MUL16);
////    adc_oversample_mode_enable(ADC0);
//	
//    /* ADC channel length config */
//    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 8);
//    /* ADC regular channel config */
//    adc_regular_channel_config(ADC0,  0, ADC_CHANNEL_0, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  1, ADC_CHANNEL_1, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  2, ADC_CHANNEL_2, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  3, ADC_CHANNEL_3, ADC_SAMPLETIME_7POINT5);   
//		adc_regular_channel_config(ADC0,  4, ADC_CHANNEL_4, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  5, ADC_CHANNEL_5, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  6, ADC_CHANNEL_6, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  7, ADC_CHANNEL_7, ADC_SAMPLETIME_7POINT5);

//    /* ADC trigger config *///规则通道组， 软件触发
//    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
//    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

//    /* enable ADC interface */
//    adc_enable(ADC0);
//    delay_1ms(100);
//    /* ADC calibration and reset calibration */
//    adc_calibration_enable(ADC0);
//		
//    /* ADC DMA function enable */
//    adc_dma_mode_enable(ADC0);

//    /* ADC software trigger enable */
//    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
//}
uint16_t adc_value[8];//ADC采样值
uint16_t Vol_Value[3];//电压值
/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;
    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);//ADC0需要映射到通道0
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&Vol_Value);//采样值存储到变量
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = 3;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);
    dma_circulation_enable(DMA0, DMA_CH0);//循环模式使能
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);//使能DMA0通道0
}

void adc_config(void)
{ /* reset ADC */
    adc_deinit(ADC0);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC contineous function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 3);
//    /* ADC regular channel config */adc_calibration_enable
//    adc_regular_channel_config(ADC0,  0, ADC_CHANNEL_0, ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,  0, ADC_CHANNEL_1, ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,  1, ADC_CHANNEL_2, ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,  2, ADC_CHANNEL_3, ADC_SAMPLETIME_7POINT5);   
//		adc_regular_channel_config(ADC0,  1, ADC_CHANNEL_4, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  0, ADC_CHANNEL_5, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  1, ADC_CHANNEL_6, ADC_SAMPLETIME_7POINT5);
//    adc_regular_channel_config(ADC0,  2, ADC_CHANNEL_7, ADC_SAMPLETIME_7POINT5);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    
    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(100);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
		delay_1ms(100);
    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);//DMA使能
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL); 
}

void adc_5init(void)
{
	
	//配置PA4 ADC引脚
	rcu_config();
	systick_config();
//	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);  
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3);
//	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_4);  
//	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_5);
//	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_6);
//	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_7);
	//配置ADC
	dma_config();//DMA配置
	adc_config();//ADC配置
		
}
uint8_t return_adc_value(){
	return Vol_Value[2]/16;
}
uint16_t return_controller_key(){
	return Vol_Value[1];
}

float rerturn_adc_out_get(void){
	float adc_out = ((Vol_Value[1]*2.0) /4096.0) *3.3;
//	printf("%4f",adc_out);
	return adc_out;
}

