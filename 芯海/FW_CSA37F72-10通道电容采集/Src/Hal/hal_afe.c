/**
* @file                hal_afe.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup HAL层
  * @{
  */
/**
 * @defgroup AFE模块
 * @brief AFE模块
 * @details 
 * @{
 */
/* Includes ------------------------------------------------------------------*/

//standard
#include <stdint.h>
#include <string.h>

//driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"

#include "csa37f7x_afe2.h"
#include "csa37f7x_i2c.h"

#include "csa37f7x_syscfg.h"
#include "csa37f7x_misc.h"

//hal
#include "hal_afe.h"
#include "hal_wdt.h"
#include "hal_pmu.h"
#include "hal_timer.h"
#include "hal_uart.h"

//debug
#include "framework_sample.h"
#include "framework_debug.h"

//config
#include "hal_config.h"

uint16_t g_afe_channel[CH_NUM] = AFE_CHANNEL_CONFIG;
uint8_t g_ainn_channel[CH_NUM] = CHANNEL_N_CONFIG;
uint8_t g_ainp_channel[CH_NUM] = CHANNEL_P_CONFIG;

#define ABS(x)  (((x)>0) ? (x) : (0-(x)))
#define BIT(X) ((uint32_t)(1)<<(X))

#define DAC_PHY2LOGIC(x)    ((x)>2048?(~((x) &(2048-1))+1):(x))
#define DAC_LOGIC2PHY(x)    ((x)<0?(2048-(x)):(x))

volatile uint8_t AFE0_Finish = 0;

static int16_t s_adc_accumulate[CH_MAX] = {0};

uint16_t DACOffset[CH_NUM] = {0x00};
uint16_t ADCOffset[CH_NUM] = {0x00};

int16_t hal_afe_rawdata[CH_NUM];

static uint8_t hal_afe_flag_adc_overflow = 0;

static void hal_afe_wait_sample_finished(uint8_t afe_num);
static void hal_afe_set_dac_ch(uint8_t AFE_Channelx, uint16_t val);
static void hal_afe_calibration_all(void);
static int16_t hal_afe_data_get(uint8_t afe_num, uint8_t ChannelIdx);

/**
* @fn static void hal_afe_calibration_all(void)
* @brief calibration all ch
* @param None
* @return None
*/
static void hal_afe_calibration_all(void)       // DAC 校正所有通道
{
    uint8_t ch;

    for(ch = 0; ch < CH_NUM; ch++)
    {
        hal_afe_calibration_ch(g_afe_channel, ch);
    }
}

/**
* @fn static void hal_afe_calibration_ch(uint8_t afe_num, uint8_t ChannelIdx)
* @brief calibration one ch
* @param [in]afe_num:AFE num, [in]ChannelIdx: ch num, [in]first_flag: 1:first time
* @return None
*/
void hal_afe_calibration_ch(uint16_t *afe_channel_t, uint8_t ChannelIdx)     // DAC校正单通道
{
    uint16_t ADC_Val = 0x00;
    uint16_t dac_logic = 0;
    uint8_t iteration_cnt;
    uint32_t pre_cfg = AFE->ADCHER;                                     //保存之前的通道配置

    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_0,DISABLE);           //暂时关闭其他无需校准的通道
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_1,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_2,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_3,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_4,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_5,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_6,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_7,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_8,DISABLE);
    afe2_adc_channel_enable_ctrl(AFE2_AIN_Channel_9,DISABLE);
    afe2_adc_channel_enable_ctrl(g_afe_channel[ChannelIdx],ENABLE);         //使能需要校准的通道
    hal_timer_delay_us(10);

    afe2_dac_data_set(afe_channel_t[ChannelIdx], 0);                        //初始化DAC值为0
    afe2_dac_channel_enable_ctrl(afe_channel_t[ChannelIdx],ENABLE);          //DAC_ENn(通道n DAC使能)

    afe2_adc_conversion_start();//开始ADC转换
    while(afe2_adc_result_valid_flag_get(afe_channel_t[ChannelIdx])==RESET); //等待转换完成
    ADC_Val = afe2_adc_conversion_value_get(afe_channel_t[ChannelIdx]);      //获取码值
    if((ADC_Val & BIT(15)) == 0)                                             //判断失调电压正负
    {
        dac_logic |= BIT(11);
    }
    dac_logic |= BIT(10);
    afe2_dac_data_set(afe_channel_t[ChannelIdx], dac_logic);
    afe2_dac_channel_enable_ctrl(afe_channel_t[ChannelIdx],ENABLE);         //DAC_ENn(通道n DAC使能)
    for(iteration_cnt = 10; iteration_cnt > 0; iteration_cnt--)             //二分法确认DAC值
    {
        afe2_adc_conversion_start();//开始ADC转换
        while(afe2_adc_result_valid_flag_get(afe_channel_t[ChannelIdx])==RESET); //等待转换完成
        ADC_Val = afe2_adc_conversion_value_get(afe_channel_t[ChannelIdx]); //获取码值
        if((ADC_Val >> 15) == (dac_logic >> 11))
        {
            dac_logic &= ~BIT(iteration_cnt);
        }
        dac_logic |= BIT(iteration_cnt - 1);
        afe2_dac_data_set(afe_channel_t[ChannelIdx], dac_logic);
        afe2_dac_channel_enable_ctrl(afe_channel_t[ChannelIdx],ENABLE);     //DAC_ENn(通道n DAC使能)
    }
    DACOffset[ChannelIdx] = DAC_PHY2LOGIC(dac_logic);
    ADCOffset[ChannelIdx] = ADC_Val;
    afe2_adc_channel_enable_ctrl(afe_channel_t[ChannelIdx],DISABLE);        // 关闭校准通道
    afe2_adc_channel_enable_ctrl(pre_cfg,ENABLE);                           // 还原原通道开关设定
}

/**
* @fn static void hal_afe_wait_sample_finished(uint8_t afe_num)
* @brief wait afe sample finish
* @param [in]afe_num:AFE num
* @return None
*/
static void hal_afe_wait_sample_finished(uint8_t afe_num)       // 等待AFE采样完成
{
#if HAL_AFE_INT_ENBALE
    do{      
        if(i2c_flag_status_get(I2C_STS_BUSYF) == RESET)
    {
            hal_pmu_enter_deep_sleep1();
        }
    }
    while(AFE0_Finish == 0);
    AFE0_Finish = 0;
#else
    while(afe_adc_conver_status_get(ADC_CONVER_CTRL) == 0);
#endif

}

/**
* @fn static int16_t hal_afe_data_get(uint8_t afe_num, uint8_t ChannelIdx)
* @brief get afe sample data
* @param [in]afe_num:AFE num, [in]ChannelIdx: ch num
* @return None
*/
static int16_t hal_afe_data_get(uint8_t afe_num, uint8_t ChannelIdx)    // 获取AFE采样数据
{
    uint16_t temp;
    int16_t val = 0;
    uint8_t chn = g_afe_channel[ChannelIdx];

    temp = afe2_adc_conversion_value_get(chn);

    val = temp>>2;

    return val;
}

/**
* @fn static void hal_afe_enable(uint8_t afe_num)
* @brief AFE enable
* @param [in]afe_num:AFE num
* @return None.
*/
static void hal_afe_enable(uint8_t afe_num)             // AFE 相关功能使能，包含PGA,DAC,VS
{
    afe2_pga_enable_ctrl(ENABLE);//open PGA
    afe2_adc_enable_ctrl(ENABLE);//open ADC
    //afe2_dac_enable_ctrl(ENABLE);//open DAC
    afe2_vs_enable_ctrl(ENABLE);//open  VS
}
/**
* @fn static void hal_afe_disable(uint8_t afe_num)
* @brief AFE disable
* @param [in]afe_num:AFE num
* @return None.
*/
static void hal_afe_disable(uint8_t afe_num)            // AFE 相关功能关闭，包含PGA,DAC,VS
{
    afe2_pga_enable_ctrl(DISABLE);//close PGA
    afe2_adc_enable_ctrl(DISABLE);//close ADC
    afe2_dac_enable_ctrl(DISABLE);//close DAC
    afe2_vs_enable_ctrl(DISABLE);//close  VS
}

/**
* @fn static void hal_afe_set_dac_ch(uint8_t AFE_Channelx, uint16_t val)
* @brief set DAC value
* @param [in]afe_num:AFE num, [in]AFE_Channelx: ch num, [in]val: data
* @return None.
*/
static void hal_afe_set_dac_ch(uint8_t AFE_Channelx, uint16_t val)     // 设置通道DAC 寄存器值
{
    afe2_dac_data_set(AFE_Channelx,val);
}

/**
* @fn static void hal_afe_sample_chx(uint8_t afe_num, uint8_t channelIdx)
* @brief sample ch
* @param [in]afe_num:AFE num, [in]channelIdx: ch num
* @return None.
*/
static void hal_afe_sample_chx(uint8_t afe_num, uint8_t channelIdx)         // 使能通道采样并等待完成
{
    afe2_adc_channel_enable_ctrl(g_afe_channel[channelIdx],ENABLE);         // 设置采样通道
    afe2_adc_conversion_start();                                            // 启动采样
    hal_afe_wait_sample_finished(afe_num);                                  // 等待采样完成
    afe2_adc_channel_enable_ctrl(g_afe_channel[channelIdx],DISABLE);        // 关闭采样通道
}

/**
* @fn uint8_t hal_afe_info_get(uint8_t *info_data)
* @brief get age sample info
* @param [in]info_data: point to data
* @return data num
*/
uint8_t hal_afe_info_get(uint8_t *info_data)                                // 获取AFE信息数据，包含PGA,DAC offset，ADC offset
{
    char i;

    info_data[0] = HAL_AFE_PGA1_GAIN;
    info_data[1] = HAL_AFE_PGA2_GAIN;
    /* CH_NUM数据被下面的offset覆盖了，不再使用 */
    /* 通道数以获取动态数据中的通道数为准 */
    info_data[2] = CH_NUM;

    for(i = 0; i < CH_NUM; i++)
    {
        info_data[4 * i + 2] = (uint8_t)DACOffset[i];
        info_data[4 * i + 3] = (uint8_t)(DACOffset[i] >> 8);
        info_data[4 * i + 4] = (uint8_t) ADCOffset[i];
        info_data[4 * i + 5] = (uint8_t)(ADCOffset[i] >> 8);
    }

    return (CH_NUM * 4 + 2);
}

/**
* @fn void hal_afe_interrput(void)
* @brief afe interrupt fun
* @param None
* @return None.
*/
void hal_afe_interrput(void)                // AFE 采样中断调用函数
{
    afe2_interrupt_flag_clear();
    AFE0_Finish = 1;
}

/**
* @fn hal_afe_ch_num_get(void)
* @brief get ch num
* @param None
* @return ch num
*/
uint8_t hal_afe_ch_num_get(void)            // 获取采样通道数
{
    return CH_NUM;
}
/**
* @fn void afe_adc_calibration(void)
* @brief afe adc calibration
* @param None
* @return None
*/
static uint8_t afe_adc_calibration(void)
{
//afe2_vs_voltage_set(HAL_AFE_VS);						//VS level  28.V
    afe2_adc_enable_ctrl(ENABLE);                       //open ADC
    afe2_vs_enable_ctrl(ENABLE);                        //open VS

    //Enable ADC
    afe2_adc_calibration_start();
    while(afe2_flag_status_get(AFE2_FLAG_CALF) == 0);   //wait for calibration finish
    afe2_flag_clear(AFE2_FLAG_CALF);                    //清除校准完成标志位
    return afe2_flag_status_get(AFE2_FLAG_CAL_VALID);
}

/**
* @fn void hal_afe_sample_param_init(void)
* @brief init sample param
* @param None
* @return None
*/
static void hal_afe_sample_param_init()
{
    uint8_t i;

    afe2_config_t p_afe_config;
    for(i = 0; i < CH_NUM; i++)                                  // 采样的通道配置
    {
        afe2_adc_channel_config(g_afe_channel[i], g_ainp_channel[i], g_ainn_channel[i]);
    }
    p_afe_config.data_format = 0;
    p_afe_config.sample_time = AFE2_SAMPLE_TIME_8Cycles;
    p_afe_config.average_num = AFE2_AVERAGE_NUM_16;	             //16次有效转换周期
    p_afe_config.stop_mode = 0;
    p_afe_config.setup_time_int = AFE2_SETUP_TIME_240Cycles;    //20us建立时间
    p_afe_config.average_mode = AFE2_AVERAGE_MODE_TRIM;
    p_afe_config.conv_mode = AFE2_CONV_MODE_SINGLE_SCAN;//AFE2_CONV_MODE_LIMITED_SCAN;  //有限周期扫描
    p_afe_config.power_mode = AFE2_POWER_MODE_LP;
    afe2_init(&p_afe_config);

    for(i = 0; i < CH_NUM; i++)                                  // 采样的通道配置
    {
        afe2_pga_gain_set(g_afe_channel[i],HAL_AFE_PGA1_GAIN,HAL_AFE_PGA2_GAIN);
    }
    afe2_pga_enable_ctrl(ENABLE);
}
/**
* @fn void hal_afe_init(void)
* @brief afe init
* @param None
* @return ch num
*/
void hal_afe_init(void)                                         // AFE模块初始化，包含通道DAC校正
{
    static uint8_t valid;
    gpio_config_t gpio_config;
    nvic_config_t ptr_config;
    uint32_t i;

    for(i = 0; i < CH_NUM; i++)
    {
        gpio_mf_config(GPIO_GROUP_A, GPIO_PIN_NUM0,GPIO_MUX_FUNC_1);            //VS0
        // 采样通道的IO配置
        if((g_ainn_channel[i] == AFE2_AINN_SOURCE_FT10) || (g_ainp_channel[i] == AFE2_AINP_SOURCE_FT10 )){
            
            gpio_mf_config(GPIO_GROUP_B, GPIO_PIN_NUM4, GPIO_MUX_FUNC_3);
            gpio_mf_config(GPIO_GROUP_B, GPIO_PIN_NUM5, GPIO_MUX_FUNC_5);            
            gpio_config.mode = GPIO_MODE_AN;
            gpio_config.pin = GPIO_PIN_4;
            gpio_config.pull = GPIO_PULL_NO_PULL;
            gpio_init(GPIO_GROUP_B,&gpio_config);
        }
        if((g_ainn_channel[i] == AFE2_AINN_SOURCE_FT11) || (g_ainp_channel[i] == AFE2_AINP_SOURCE_FT11 )){
            gpio_mf_config(GPIO_GROUP_B, GPIO_PIN_NUM5, GPIO_MUX_FUNC_5);
            gpio_config.mode = GPIO_MODE_AN;
            gpio_config.pin = GPIO_PIN_5;
            gpio_config.pull = GPIO_PULL_NO_PULL;
            gpio_init(GPIO_GROUP_B,&gpio_config);            
        }
    }
    syscfg_regwrprot_disable();
    rcc_hirc_adc_enable_ctrl(ENABLE);                                           // 使能内部 24Mhz 高速振荡器
    syscfg_regwrprot_enable();
    rcc_apb_periph_clock_enable_ctrl(RCC_APBPeriph_AFE2, ENABLE);               // AFE ADC 时钟使能控制

    afe2_vs_voltage_set(HAL_AFE_VS);                                            //VS level  28.V
    hal_afe_sample_param_init();    
    afe2_vs_limit_enable_ctrl(ENABLE);
//  afe2_vs_output_enable_ctrl(ENABLE);             // VS_SEL(由硬件控制VS0输出开启或关闭,只在转换FT通道时开启)
    afe2_vs_enable_ctrl(ENABLE);                    // VS_EN
    hal_timer_delay_ms(6);
    afe2_vs_limit_enable_ctrl(DISABLE);
    hal_timer_delay_ms(1);
    do
    {
        valid = afe_adc_calibration();				// adc calibration
    }while(valid == 0);
#if HAL_AFE_INT_ENBALE                                                          // 选择是否使能AFE采样完成中断  
    ptr_config.nvic_channel_priority = 1;
    ptr_config.nvic_enable_flag = ENABLE;
    ptr_config.nvic_IRQ_channel = IRQn_AFE2;
    nvic_init(&ptr_config);
    afe2_adc_interrupt_enable_ctrl(ENABLE);
#else
    afe_adc_interrupt_disable();
#endif       
    for(i = 0; i < CH_NUM; i++){                    // 采样的通道配置
        afe2_pga_bypass_set(g_afe_channel[i],AFE2_PGA_BYPASS_NONE);   // default config AA bypass
    }											   
    //--dac---------------------------------------------
    afe2_dac_vref_set(AFE2_DAC_VREF_2VS);           //DAC 输出 (NP) rang = 800mV

    afe2_pga_enable_ctrl(ENABLE);                   //open PGA
    afe2_vs_enable_ctrl(ENABLE);                    //open  VS
    afe2_adc_enable_ctrl(ENABLE);                   //open ADC
    //afe2_dac_enable_ctrl(ENABLE);
    hal_afe_calibration_all();                      //DAC校正使能的通道
    afe2_vs_enable_ctrl(DISABLE);
}

/**
* @fn int16_t hal_afe_get_rawdata(int ch)
* @brief get afe sample data
* @param [in]ch: ch num
* @return sample data
*/
extern int16_t calibrate_offset[];
int16_t hal_afe_get_rawdata(int ch)                 //获取指定的通道采样值
{
    if(ch < CH_NUM)
    {
        return hal_afe_rawdata[ch] - BASE_LINE + calibrate_offset[TK_CH_NUM+ch];
    }
    return 0;
}

/**
* @fn uint8_t hal_afe_get_adc_overflow(void)
* @brief
* @param
* @return
*/
uint8_t hal_afe_get_adc_overflow(void)                             // 获取ADC累加是否溢出标记
{
    return hal_afe_flag_adc_overflow;
}

/**
* @fn void hal_afe_sample(void)
* @brief sample proc
* @param None
* @return None.
*/
uint16_t hal_afe_temperature_sample(uint8_t ChannelIdx)         // 启动温度采样
{
    uint16_t adc_value;

    hal_afe_enable(HAL_AFE_NUM);                                //  开启AFE设置
    afe2_adc_channel_enable_ctrl(ChannelIdx,ENABLE);            // 设置采样通道
    hal_timer_delay_us(10);    
    afe2_adc_conversion_start();                                // 启动采样
    hal_afe_wait_sample_finished(0);                            // 等待采样完成
    afe2_adc_channel_enable_ctrl(ChannelIdx,DISABLE);           // 关闭采样通道
    adc_value = afe2_adc_conversion_value_get(ChannelIdx);
    adc_value = (adc_value>>2);
    hal_afe_disable(HAL_AFE_NUM);                               //  关闭AFE设置
    return adc_value;
}


/**
* @fn void hal_afe_sample(void)
* @brief sample proc
* @param None
* @return None.
*/
void hal_afe_sample(void)                                       // 启动AFE采样
{
    char i;
    int16_t adc_value;

    hal_afe_flag_adc_overflow = 0;
    hal_afe_enable(HAL_AFE_NUM);                                // 开启AFE设置
    for(i = 0; i < CH_NUM; i++)                                 // 依次采样使能的通道
    {
        hal_afe_sample_chx(HAL_AFE_NUM, i);
        adc_value = hal_afe_data_get(HAL_AFE_NUM, i);
        hal_afe_rawdata[i] = adc_value + s_adc_accumulate[i];   // 计算增加累加量修正的采样值
    }
    hal_afe_disable(HAL_AFE_NUM);                               // 关闭AFE设置
}

int16_t get_adc_accumulate_data(uint8_t ch)
{
    return s_adc_accumulate[ch];    
}
/**
* @fn uint8_t get_comparing_values(int16_t value)
* @brief ger adc value comparing status
* @param [in]adc value: ch num
* @return status
*/
static uint8_t hal_get_comparing_values(int16_t value)
{
    uint8_t status;

    if(value < 0x0800)
    {
        status = 1;      //    (-VS)---(-0.25VS)
    }
    else if(value > 0x07FF && value < 0x1800)
    {
        status = 2;      //    (-0.25VS)---(-0.75VS)
    }
    else if(value > 0x17FF && value < 0x2800)
    {
        status = 3;      //    (-0.75VS)---(+0.25VS)
    }
    else if(value > 0x27FF && value < 0x3800)
    {
        status = 4;      //    (+0.25VS)---(+0.75VS)
    }
    else if(value > 0x37FF && value < 0x4001)
    {
        status = 5;      //    (+0.75VS)---(+VS)
    }
    else
    {
        status = 6;      //    other situation
    }
    return status;
}
/**
* @fn uint16_t hal_get_sensor_state( uint8_t ChannelIdx)
* @brief None
* @param None
* @return None.
*/
uint16_t hal_get_sensor_state( uint8_t ChannelIdx)
{
    uint8_t i;
    uint16_t sensor_status = 0;
    uint8_t subType[6] = {0};
    int16_t adc_value[6] = {0};

    afe2_pga_bypass_set(g_afe_channel[ChannelIdx],AFE2_PGA_BYPASS_BOTH);
    afe2_adc_channel_config(g_afe_channel[ChannelIdx], g_ainn_channel[ChannelIdx],AFE2_AINP_SOURCE_HALF_VS);//设置 P端 接VS/2
    //  测试P半桥
    hal_afe_enable(HAL_AFE_NUM);                                    // 开启AFE设置
    afe2_pull_up_enable_ctrl(ENABLE);
    afe2_pull_down_enable_ctrl(DISABLE);
    hal_timer_delay_ms(5);
    hal_afe_sample_chx(HAL_AFE_NUM, ChannelIdx);
    adc_value[0] = hal_afe_data_get(HAL_AFE_NUM, ChannelIdx);

    afe2_pull_up_enable_ctrl(DISABLE);
    afe2_pull_down_enable_ctrl(ENABLE);
    hal_timer_delay_ms(5);
    hal_afe_sample_chx(HAL_AFE_NUM, ChannelIdx);
    adc_value[1] = hal_afe_data_get(HAL_AFE_NUM, ChannelIdx);

    afe2_pull_up_enable_ctrl(DISABLE);
    afe2_pull_down_enable_ctrl(DISABLE);
    hal_timer_delay_ms(5);
    hal_afe_sample_chx(HAL_AFE_NUM, ChannelIdx);
    adc_value[2] = hal_afe_data_get(HAL_AFE_NUM, ChannelIdx);

    // 测试N半桥
    afe2_adc_channel_config(g_afe_channel[ChannelIdx], g_ainp_channel[ChannelIdx], AFE2_AINP_SOURCE_HALF_VS); //设置 N端 接VS/2

    afe2_pull_up_enable_ctrl(ENABLE);
    afe2_pull_down_enable_ctrl(DISABLE);
    hal_timer_delay_ms(5);
    hal_afe_sample_chx(HAL_AFE_NUM, ChannelIdx);
    adc_value[3] = hal_afe_data_get(HAL_AFE_NUM, ChannelIdx);

    afe2_pull_up_enable_ctrl(DISABLE);
    afe2_pull_down_enable_ctrl(ENABLE);
    hal_timer_delay_ms(5);
    hal_afe_sample_chx(HAL_AFE_NUM, ChannelIdx);
    adc_value[4] = hal_afe_data_get(HAL_AFE_NUM, ChannelIdx);

    afe2_pull_up_enable_ctrl(DISABLE);
    afe2_pull_down_enable_ctrl(DISABLE);
    hal_timer_delay_ms(5);
    hal_afe_sample_chx(HAL_AFE_NUM, ChannelIdx);
    adc_value[5] = hal_afe_data_get(HAL_AFE_NUM, ChannelIdx);

    hal_afe_disable(HAL_AFE_NUM);                                    // 关闭AFE设置

    // 恢复AFE设置
    afe2_pull_up_enable_ctrl(DISABLE);
    afe2_pull_down_enable_ctrl(DISABLE);
    afe2_pga_bypass_set(g_afe_channel[ChannelIdx],AFE2_PGA_BYPASS_NONE);
    hal_afe_sample_param_init();

    for(i = 0; i < 6; i++)
    {
        subType[i] = hal_get_comparing_values(adc_value[i]);
    }

    if((subType[0] == 4) && (subType[1] == 2))
    {
        sensor_status = 0x01;       //R1-OPEN      R2-OPEN
    }
    else if((subType[0] == 2) && (subType[1] == 2)  && (subType[2] == 2))
    {
        sensor_status = 0x02;       //R1-OPEN/NORMAL/SHORT    R2-SHORT
    }
    else if((subType[0] == 3) && (subType[1] == 2)  && (subType[2] == 2))
    {
        sensor_status = 0x03;       //R1-OPEN      R2-NORMAL
    }
    else if((subType[0] == 4) && (subType[1] == 3)  && (subType[2] == 4))
    {
        sensor_status = 0x04;       //R1-NORMAL    R2-OPEN
    }
    else if((subType[0] == 3) && (subType[1] == 3)  && (subType[2] == 3))
    {
        sensor_status = 0x05;       //R1-NORMAL    R2-NORMAL
    }
    else if((subType[0] == 4) && (subType[1] == 4)  && (subType[2] == 4))
    {
        sensor_status = 0x06;       //R1-SHORT     R2-NORMAL/OPEN
    }
    else
    {
        sensor_status = 0x07;       //    other situation
    }

    if((subType[3] == 4) && (subType[4] == 2))
    {
        sensor_status |= 0x10;      //R3-OPEN      R4-OPEN
    }
    else if((subType[3] == 2) && (subType[4] == 2)  && (subType[5] == 2))
    {
        sensor_status |= 0x20;      //R3-OPEN/NORMAL/SHORT    R4-SHORT
    }
    else if((subType[3] == 3) && (subType[4] == 2)  && (subType[5] == 2))
    {
        sensor_status |= 0x30;      //R3-OPEN      R4-NORMAL
    }
    else if((subType[3] == 4) && (subType[4] == 3)  && (subType[5] == 4))
    {
        sensor_status |= 0x40;      //R3-NORMAL    R4-OPEN
    }
    else if((subType[3] == 3) && (subType[4] == 3)  && (subType[5] == 3))
    {
        sensor_status |= 0x50;      //R3-NORMAL    R4-NORMAL
    }
    else if((subType[3] == 4) && (subType[4] == 4)  && (subType[5] == 4))
    {
        sensor_status |= 0x60;      //R3-SHORT     R4-NORMAL/OPEN
    }
    else
    {
        sensor_status |= 0x70;      //    other situation
    }
    return sensor_status;
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
