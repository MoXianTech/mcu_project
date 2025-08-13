/**
* @file                hal_tk.c
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
 * @defgroup TK驱动
 * @brief TK驱动
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
#include "csa37f7x_tk.h"
#include "csa37f7x_syscfg.h"
#include "csa37f7x_misc.h"
#include "csa37f7x_pwr.h"
//hal
#include "hal_tk.h"
#include "hal_afe.h"
#include "hal_timer.h"
#include "hal_pmu.h"
#include "hal_gpio.h"
//debug
#include "framework_debug.h"
#include "framework_pmu.h"

//config
#include "hal_config.h"

static uint16_t hal_afe_tk_data_get(uint8_t afe_num, uint8_t ChannelIdx);
uint16_t tk_g_afe_channel[TK_CH_NUM] = TK_CHANNEL_CONFIG;
uint8_t tk_g_ainn_channel[TK_CH_NUM] = TK_CHANNEL_N_CONFIG;
uint8_t tk_g_ainp_channel[TK_CH_NUM] = TK_CHANNEL_P_CONFIG;

static uint16_t cpin = 0;
static uint16_t Cpin_value[TK_CH_NUM] = {0};
tk_data_config_t tk_data_config;


static int16_t s_tk_adc_accumulate[TK_CH_MAX] = {0};

int16_t hal_tk_afe_rawdata[TK_CH_NUM];
static int16_t hal_tk_afe_rawdata_old[TK_CH_NUM];
volatile uint8_t tk_Finish = 0;

/**
  * @fn void afe2_tk_input_calibration_value_set(uint8_t channel, uint8_t factor)
  * @brief  TK input capcitance calibration factor set.
  * @param  channel: TK input channel 0 ~ 11.
  * @param  value: 2-bit calibration factor.
  * @return None
  */
void afe2_tk_input_calibration_set(uint8_t channel, uint8_t factor)
{
    ASSERT(AFE2_TK_CHANNEL_CHECK(channel));
    ASSERT(AFE2_TK_CPIN_FACTOR_CHECK(factor));

    AFE2->CAPDACR[channel] &= ~AFE2_CAPDACR_CPIN_DACx;
    AFE2->CAPDACR[channel] |= factor << 16;
}

/**
  * @fn void afe2_tk_ref_calibration_value_set(uint8_t channel, uint16_t factor)
  * @brief  TK reference capcitance calibration factor set.
  * @param  channel: TK input channel 0 ~ 11.
  * @param  value: 15-bit calibration factor.
  * @return None
  */
void afe2_tk_ref_calibration_set(uint8_t channel, uint16_t factor)
{
    ASSERT(AFE2_TK_CHANNEL_CHECK(channel));
    ASSERT(AFE2_TK_CREF_FACTOR_CHECK(factor));

    AFE2->CAPDACR[channel] &= ~AFE2_CAPDACR_CREF_DACx;
    AFE2->CAPDACR[channel] |= factor;
}

/**
  * @fn void afe2_tk_input_calibration_value_set(uint8_t channel, uint8_t factor)
  * @brief  TK input capcitance calibration factor set.
  * @param  channel: TK input channel 0 ~ 11.
  * @param  value: 2-bit calibration factor.
  * @return None
  */
uint8_t afe2_tk_input_calibration_get(uint8_t channel)
{
    ASSERT(AFE2_TK_CHANNEL_CHECK(channel));

    return ((uint8_t)((AFE2->CAPDACR[channel] & AFE2_CAPDACR_CPIN_DACx)>>16));
}
/**
  * @fn void afe2_tk_ref_calibration_value_set(uint8_t channel, uint16_t factor)
  * @brief  TK reference capcitance calibration factor set.
  * @param  channel: TK input channel 0 ~ 11.
  * @param  value: 15-bit calibration factor.
  * @return None
  */
uint16_t afe2_tk_ref_calibration_get(uint8_t channel)
{
    ASSERT(AFE2_TK_CHANNEL_CHECK(channel));

    return ((uint16_t)(AFE2->CAPDACR[channel] & AFE2_CAPDACR_CREF_DACx));
}
/**
* @fn  uint16_t afe2_tk_capacitance_calibration(uint8_t tk_channel,uint8_t channel_enable_sel,uint8_t ainp_source_sel,uint8_t ainn_source_sel,uint8_t vs_sel,enable_state_t pga_enable,uint8_t gain1,uint8_t gain2)
* @param  tk_channel：AFE2_TK_Channel_0~AFE2_TK_Channel_11
* @param  channel_enable_sel：AFE2_AIN_Channel_0~AFE2_AIN_Channel_9
* @param  ainp_source_sel：AFE2_AINP_SOURCE_FT0~AFE2_AINP_SOURCE_FLOATING
* @param  ainn_source_sel：AFE2_AINN_SOURCE_FT0~AFE2_AINN_SOURCE_FLOATING
* @param  vs_sel：AFE2_VS_VOLTS_2V0~AFE2_VS_VOLTS_2V8
* @param  pga_enable：ENABLE/DISABLE
* @param  gain1：AFE2_PGA1_GAIN_1~AFE2_PGA1_GAIN_128
* @param  gain2：AFE2_PGA2_GAIN_1~AFE2_PGA2_GAIN_4
*/
uint16_t afe2_tk_capacitance_calibration(uint8_t tk_channel,uint8_t channel_enable_sel)
{
    static uint16_t tk_value = 0;//,cpin = 0;
    uint16_t data_code = 0,temp = 0;

    afe2_tk_emi_current_set(AFE2_TK_EMI_CURRENT_80uA);  //REMI_SEL(TK模块中EMI性能调节信号80uA)
    afe2_tk_charge_time_set(AFE2_TK_CHARGE_TIME_7_3us); //CHARGE(TK充电时间为7.3us)
    afe2_tk_shield_enable_ctrl(ENABLE);                 //SHIELD(TK正常工作)

    hal_timer_delay_us(10);//10uS

    afe2_tk_input_calibration_set(tk_channel,0x0);      //CPIN_DACx(输入电容校准值)
    afe2_tk_ref_calibration_set(tk_channel,0x4000);     //CREF_DACx(参考电容校准值)
    for(int i = 14;i>=0;i--)
    {
        afe2_adc_conversion_start();//开始ADC转换
        pwr_sleep_mode_enter(PWR_SLEEP_MODE_DEEP1,PWR_SLEEP_ENTRY_WFI);//enter deepsleep1 mode
        while(afe2_adc_result_valid_flag_get(channel_enable_sel)==RESET);//等待转换完成
        data_code = afe2_adc_conversion_value_get(channel_enable_sel);//获取码值
        temp = afe2_tk_ref_calibration_get(tk_channel);
        tk_value = (temp & ~BIT(i)) | ((data_code>>15)<<i) | BIT(i-1);
        afe2_tk_ref_calibration_set(tk_channel,tk_value);//配置CREF_DACx
    }
    cpin = 34*((tk_value & BIT(14))>>14) + 32*((tk_value & BIT(13))>>13) + 16*((tk_value & BIT(12))>>12) + 8*((tk_value & BIT(11))>>11) + 4*((tk_value & BIT(10))>>10) + 2*((tk_value & BIT(9))>>9) + 1*((tk_value & BIT(8))>>8) + 1.6*((tk_value & BIT(7))>>7) + 0.8*((tk_value & BIT(6))>>6) + 0.64*((tk_value & BIT(5))>>5) + 0.32*((tk_value & BIT(4))>>4) + 0.16*((tk_value & BIT(3))>>3) + 0.08*((tk_value & BIT(2))>>2) + 0.04*((tk_value & BIT(1))>>1) + 0.025*((tk_value & BIT(0))>>0);//外部等效输入电容计算
    if((((tk_value & BIT(14))>>14) == 0)&&(((tk_value & BIT(13))>>13) == 0))
    {
        afe2_tk_input_calibration_set(tk_channel,0x2);//CPIN_DACx(输入电容校准值)
        afe2_tk_ref_calibration_set(tk_channel,0x4000);//CREF_DACx(参考电容校准值)
        for(int i = 14;i>=0;i--)
        {
            afe2_adc_conversion_start();//开始ADC转换
            pwr_sleep_mode_enter(PWR_SLEEP_MODE_DEEP1,PWR_SLEEP_ENTRY_WFI);//enter deepsleep1 mode
            while(afe2_adc_result_valid_flag_get(channel_enable_sel)==RESET);//等待转换完成
            data_code = afe2_adc_conversion_value_get(channel_enable_sel);//获取码值
            temp = afe2_tk_ref_calibration_get(tk_channel);
            tk_value = (temp & ~BIT(i)) | ((data_code>>15)<<i) | BIT(i-1);
            afe2_tk_ref_calibration_set(tk_channel,tk_value);//配置CREF_DACx
        }
        cpin = 34*((tk_value & BIT(14))>>14) + 32*((tk_value & BIT(13))>>13) + 16*((tk_value & BIT(12))>>12) + 8*((tk_value & BIT(11))>>11) + 4*((tk_value & BIT(10))>>10) + 2*((tk_value & BIT(9))>>9) + 1*((tk_value & BIT(8))>>8) + 1.6*((tk_value & BIT(7))>>7) + 0.8*((tk_value & BIT(6))>>6) + 0.64*((tk_value & BIT(5))>>5) + 0.32*((tk_value & BIT(4))>>4) + 0.16*((tk_value & BIT(3))>>3) + 0.08*((tk_value & BIT(2))>>2) + 0.04*((tk_value & BIT(1))>>1) + 0.025*((tk_value & BIT(0))>>0);//外部等效输入电容计算
        if((62-cpin)>=10)
        {
            afe2_tk_input_calibration_set(tk_channel,0x3);//CPIN_DACx(输入电容校准值)
            afe2_tk_ref_calibration_set(tk_channel,0x4000);//CREF_DACx(参考电容校准值)
            for(int i = 14;i>=0;i--)
            {
                afe2_adc_conversion_start();//开始ADC转换
                pwr_sleep_mode_enter(PWR_SLEEP_MODE_DEEP1,PWR_SLEEP_ENTRY_WFI);//enter deepsleep1 mode
                while(afe2_adc_result_valid_flag_get(channel_enable_sel)==RESET);//等待转换完成
                data_code = afe2_adc_conversion_value_get(channel_enable_sel);//获取码值
                temp = afe2_tk_ref_calibration_get(tk_channel);
                tk_value = (temp & ~BIT(i)) | ((data_code>>15)<<i) | BIT(i-1);
                afe2_tk_ref_calibration_set(tk_channel,tk_value);//配置CREF_DACx
            }
            cpin = 34*((tk_value & BIT(14))>>14) + 32*((tk_value & BIT(13))>>13) + 16*((tk_value & BIT(12))>>12) + 8*((tk_value & BIT(11))>>11) + 4*((tk_value & BIT(10))>>10) + 2*((tk_value & BIT(9))>>9) + 1*((tk_value & BIT(8))>>8) + 1.6*((tk_value & BIT(7))>>7) + 0.8*((tk_value & BIT(6))>>6) + 0.64*((tk_value & BIT(5))>>5) + 0.32*((tk_value & BIT(4))>>4) + 0.16*((tk_value & BIT(3))>>3) + 0.08*((tk_value & BIT(2))>>2) + 0.04*((tk_value & BIT(1))>>1) + 0.025*((tk_value & BIT(0))>>0);//外部等效输入电容计算
            return tk_value;
        }
        else
        {
            return tk_value;
        }
    }
    else
    {
        if((62-cpin)>=24)
        {
            afe2_tk_input_calibration_set(tk_channel,0x2);//CPIN_DACx(输入电容校准值)
            afe2_tk_ref_calibration_set(tk_channel,0x4000);//CREF_DACx(参考电容校准值)
            for(int i = 14;i>=0;i--)
            {
                afe2_adc_conversion_start();//开始ADC转换
                pwr_sleep_mode_enter(PWR_SLEEP_MODE_DEEP1,PWR_SLEEP_ENTRY_WFI);//enter deepsleep1 mode
                while(afe2_adc_result_valid_flag_get(channel_enable_sel)==RESET);//等待转换完成
                data_code = afe2_adc_conversion_value_get(channel_enable_sel);//获取码值
                temp = afe2_tk_ref_calibration_get(tk_channel);
                tk_value = (temp & ~BIT(i)) | ((data_code>>15)<<i) | BIT(i-1);
                afe2_tk_ref_calibration_set(tk_channel,tk_value);//配置CREF_DACx
            }
            cpin = 34*((tk_value & BIT(14))>>14) + 32*((tk_value & BIT(13))>>13) + 16*((tk_value & BIT(12))>>12) + 8*((tk_value & BIT(11))>>11) + 4*((tk_value & BIT(10))>>10) + 2*((tk_value & BIT(9))>>9) + 1*((tk_value & BIT(8))>>8) + 1.6*((tk_value & BIT(7))>>7) + 0.8*((tk_value & BIT(6))>>6) + 0.64*((tk_value & BIT(5))>>5) + 0.32*((tk_value & BIT(4))>>4) + 0.16*((tk_value & BIT(3))>>3) + 0.08*((tk_value & BIT(2))>>2) + 0.04*((tk_value & BIT(1))>>1) + 0.025*((tk_value & BIT(0))>>0);//外部等效输入电容计算
            return tk_value;
        }
        else if(((62-cpin)>=8)&&((62-cpin)<24))
        {
            afe2_tk_input_calibration_set(tk_channel,0x1);//CPIN_DACx(输入电容校准值)
            afe2_tk_ref_calibration_set(tk_channel,0x4000);//CREF_DACx(参考电容校准值)
            for(int i = 14;i>=0;i--)
            {
                afe2_adc_conversion_start();//开始ADC转换
                pwr_sleep_mode_enter(PWR_SLEEP_MODE_DEEP1,PWR_SLEEP_ENTRY_WFI);//enter deepsleep1 mode
                while(afe2_adc_result_valid_flag_get(channel_enable_sel)==RESET);//等待转换完成
                data_code = afe2_adc_conversion_value_get(channel_enable_sel);//获取码值
                temp = afe2_tk_ref_calibration_get(tk_channel);
                tk_value = (temp & ~BIT(i)) | ((data_code>>15)<<i) | BIT(i-1);
                afe2_tk_ref_calibration_set(tk_channel,tk_value);//配置CREF_DACx
            }
            cpin = 34*((tk_value & BIT(14))>>14) + 32*((tk_value & BIT(13))>>13) + 16*((tk_value & BIT(12))>>12) + 8*((tk_value & BIT(11))>>11) + 4*((tk_value & BIT(10))>>10) + 2*((tk_value & BIT(9))>>9) + 1*((tk_value & BIT(8))>>8) + 1.6*((tk_value & BIT(7))>>7) + 0.8*((tk_value & BIT(6))>>6) + 0.64*((tk_value & BIT(5))>>5) + 0.32*((tk_value & BIT(4))>>4) + 0.16*((tk_value & BIT(3))>>3) + 0.08*((tk_value & BIT(2))>>2) + 0.04*((tk_value & BIT(1))>>1) + 0.025*((tk_value & BIT(0))>>0);//外部等效输入电容计算
            return tk_value;
        }
        else
        {
            return tk_value;
        }
    }
}

/**
* @fn void hal_tk_init(void)
* @brief tk init
* @param None
* @return none
*/
void hal_tk_init(void)                                         // TK模块初始化，包含通道DAC校正
{
    uint8_t i;
    afe2_config_t afe2_config_struct;
    nvic_config_t nvic_config_struct;
    gpio_config_t gpio_config;
//IO复用
//================================================================================
    for(i = 0; i < TK_CH_NUM; i++)
    {
        gpio_mf_config(GPIO_GROUP_A, GPIO_PIN_NUM0,GPIO_MUX_FUNC_1);            //VS0
        // 采样通道的IO配置
        if((tk_g_ainn_channel[i] == AFE2_AINN_SOURCE_FT10) || (tk_g_ainp_channel[i] == AFE2_AINP_SOURCE_FT10 )){
            
            gpio_mf_config(GPIO_GROUP_B, GPIO_PIN_NUM4, GPIO_MUX_FUNC_3);   
            gpio_mf_config(GPIO_GROUP_B, GPIO_PIN_NUM5, GPIO_MUX_FUNC_5);            
            gpio_config.mode = GPIO_MODE_AN;
            gpio_config.pin = GPIO_PIN_4;
            gpio_config.pull = GPIO_PULL_NO_PULL;
            gpio_init(GPIO_GROUP_B,&gpio_config);                 
        }
        if((tk_g_ainn_channel[i] == AFE2_AINN_SOURCE_FT11) || (tk_g_ainp_channel[i] == AFE2_AINP_SOURCE_FT11 )){
            gpio_mf_config(GPIO_GROUP_B, GPIO_PIN_NUM5, GPIO_MUX_FUNC_5);
            gpio_config.mode = GPIO_MODE_AN;
            gpio_config.pin = GPIO_PIN_5;
            gpio_config.pull = GPIO_PULL_NO_PULL;
            gpio_init(GPIO_GROUP_B,&gpio_config);            
        }	
    } 
    //时钟使能
    //================================================================================
    rcc_hirc_adc_enable_ctrl(ENABLE);//使能ADC专用高速振荡器
    rcc_apb_periph_clock_enable_ctrl(RCC_APBPeriph_AFE2,ENABLE);//enable AFE2 CLK
    //ADC硬件校准
    //================================================================================
    afe2_pga_enable_ctrl(DISABLE);//关闭PGA
    afe2_vs_voltage_set(AFE2_VS_VOLTS_2V8);//VS 输出档位选择 2.8V
    afe2_vs_enable_ctrl(ENABLE);//VS_EN(开启VS稳压使能控制)
    hal_timer_delay_ms(6);
    afe2_vs_limit_enable_ctrl(DISABLE);//VS正常模式
    hal_timer_delay_us(20);//10uS
    afe2_adc_enable_ctrl(ENABLE);//使能A/D转换器
    hal_timer_delay_us(20);//10uS
    afe2_adc_calibration_start();//使能ADC校准模式
    while(afe2_flag_status_get(AFE2_FLAG_CALF) != SET);//等待校准完成
    afe2_flag_clear(AFE2_FLAG_CALF);//清除校准完成标志位
    //TK配置初始化
    //================================================================================
    syscfg_ainx_mfp_config((AFE2_AINP_SOURCE_TK0-0x10),SYSCFG_AIN_MUX_FUNC_1);//AINx->TKx
    afe2_config_struct.average_mode = AFE2_AVERAGE_MODE_TRIM;//CAL_S(直接进行平均处理)(转换次数(M)=N+2)
    afe2_config_struct.average_num = AFE2_AVERAGE_NUM_2;//VALID_NS(有效转换次数N)
    afe2_config_struct.conv_mode = AFE2_CONV_MODE_SINGLE_SCAN;//AFE2_CONV_MODE_LIMITED_SCAN;//ADMD(有限周期扫描模式1)
    afe2_config_struct.data_format = AFE2_DATA_FORMAT_UNSIGNED;//DMOF(A/D转换结果无符号保存)
    afe2_config_struct.power_mode = AFE2_POWER_MODE_LP;//VCM_CFG(低功耗模式，VCM受SW_AIN控制)
    afe2_config_struct.sample_time = AFE2_SAMPLE_TIME_32Cycles;//CHSAMPCNT(通道采样计数器=8*ADC_CLK)
    afe2_config_struct.setup_time_ext = 0x0;//ECHSU(外部通道建立时间)
    afe2_config_struct.setup_time_int = AFE2_SETUP_TIME_240Cycles;//CHSU(通道建立时间选择20us=240 * ADC Clock)
    afe2_config_struct.stop_mode = AFE2_STOP_MODE_INTERRUPT;//STOP_MOD(转换结束，置起中断)
    afe2_init(&afe2_config_struct);
    //TK-ADC中断配置
    //================================================================================
    afe2_adc_interrupt_enable_ctrl(ENABLE);//使能ADAFE2_CONV_MODE_SINGLE_SCANC中断
    nvic_config_struct.nvic_channel_priority = 0x01;
    nvic_config_struct.nvic_enable_flag = ENABLE;
    nvic_config_struct.nvic_IRQ_channel = IRQn_AFE2;
    nvic_init(&nvic_config_struct);//ADC中断配置
    //TK通道 电容校准
    //================================================================================
	afe2_vs_output_enable_ctrl(DISABLE);    //VS_SEL(由硬件控制VS0输出开启或关闭,只在转换FT通道时开启)
	afe2_vs_enable_ctrl(ENABLE);            //VS_EN(开启VS稳压使能控制)	
    for(i=0; i<TK_CH_NUM; i++)
    {
        afe2_pga_gain_set(tk_g_afe_channel[i],HAL_TK_AFE_PGA1_GAIN,HAL_TK_AFE_PGA2_GAIN);//PGA1、PGA2增益选择
        afe2_adc_channel_config(tk_g_afe_channel[i],tk_g_ainp_channel[i],tk_g_ainn_channel[i]);//通道N/P端输入选择
    }
    hal_tk_calibration_all();
}


/**
* @fn static uint16_t hal_afe_tk_data_get(uint8_t afe_num, uint8_t ChannelIdx)
* @brief tk sample
* @param afe_num,ChannelIdx
* @return tk_data
*/
static uint16_t hal_afe_tk_data_get(uint8_t afe_num, uint8_t ChannelIdx)
{
    uint16_t temp;
    uint16_t val = 0;

    uint8_t chn = tk_g_afe_channel[ChannelIdx];

    temp = afe2_adc_conversion_value_get(chn);   
    val = temp >> 2;  
    return val;
}

/**
  * @fn int16_t afe2_tk_second_calibration_cdac_get(int16_t adc_value, uint16_t cpin)
  * @brief 
  * @param  int16_t adc_value, uint16_t cpin
  * @return int16_t
  */
float afe2_tk_second_calibration_cdac_get(int16_t adc_value, uint16_t cpin)
{
    double temp1,temp2,temp3;
    temp1 = (double)(adc_value - 8192)/64;
    temp1 = (double)temp1/8192;
    temp2 = (double)(cpin-24)/(double)(cpin+24);
    temp3 = temp1+temp2;
    temp1 = ((temp3*24)+24);
    temp2 = temp1/(1-temp3);
    return (float)temp2;
}
/**
* @fn static void fw_auto_calibration_proc(void)
* @brief tk auto calibration
* @param None
* @return none
*/
void fw_auto_calibration_proc_tk(void)
{
    uint8_t i = 0;
    static uint8_t ff_flag = 0;
    static uint8_t cdac_get_first_flag = 0;
    static uint8_t auto_calibration_flag[TK_CH_NUM] = {0};
    uint16_t Cref_Cdac[TK_CH_NUM] = {0};
    static float temp,temp1[TK_CH_NUM],temp2[TK_CH_NUM],temp3[TK_CH_NUM];
     
    if(cdac_get_first_flag == 0)
    {
        cdac_get_first_flag = 1;
        for(i=0; i<TK_CH_NUM; i++)
        {               
            temp1[i] = afe2_tk_second_calibration_cdac_get(hal_tk_afe_rawdata[i],Cpin_value[i]);
        }
    }

    
    for(i=0; i<TK_CH_NUM; i++)
    {    
        if(auto_calibration_flag[i] == 1)
        {         
            auto_calibration_flag[i] = 0;
            cdac_get_first_flag = 0;
            s_tk_adc_accumulate[i] = hal_tk_afe_rawdata_old[i] - hal_tk_afe_rawdata[i]; 
            if(s_tk_adc_accumulate[i]>20000) 
            {
                s_tk_adc_accumulate[i] = 0;
            }
        } 
        hal_tk_afe_rawdata[i] = hal_tk_afe_rawdata[i] + s_tk_adc_accumulate[i]; 
    } 
    if(cdac_get_first_flag == 1)        
    {
        for(i=0; i<TK_CH_NUM; i++)
        {
            if((hal_afe_tk_data_get(HAL_AFE_NUM,i)-0x2000)>4000)
            {                                         
                if(auto_calibration_flag[i] == 0)
                {
                    temp2[i] = afe2_tk_second_calibration_cdac_get(hal_tk_afe_rawdata[i],Cpin_value[i]);        
                    temp3[i] = temp2[i]-temp1[i]+Cpin_value[i];
                    Cpin_value[i] = temp3[i];
                    if(temp3[i]>34)
                    {
                        Cref_Cdac[i] |= BIT(14); 
                        temp3[i] = temp3[i] - 34;
                    }       
                    if(temp3[i]>32)
                    {
                        Cref_Cdac[i] |= BIT(13);
                        temp3[i] = temp3[i] - 32;
                    }
                    if(temp3[i]>16)
                    {
                        Cref_Cdac[i] |= BIT(12);
                        temp3[i] = temp3[i] - 16;
                    }
                    if(temp3[i]>8)
                    {
                        Cref_Cdac[i] |= BIT(11);
                        temp3[i] = temp3[i] - 8;
                    }
                    if(temp3[i]>4)
                    {
                        Cref_Cdac[i] |= BIT(10);
                        temp3[i] = temp3[i] - 4;
                    }
                    if(temp3[i]>2)
                    {
                        Cref_Cdac[i] |= BIT(9);
                        temp3[i] = temp3[i] - 2;
                    }
                    if(temp3[i]>1)
                    {
                        Cref_Cdac[i] |= BIT(8);
                        temp3[i] = temp3[i] - 1;
                    }
                    if(temp3[i]>1.6)
                    {
                        Cref_Cdac[i] |= BIT(7);
                        temp3[i] = temp3[i] - 1.6;
                    }
                    if(temp3[i]>0.8)
                    {
                        Cref_Cdac[i] |= BIT(6);
                        temp3[i] = temp3[i] - 0.8;
                    }
                    if(temp3[i]>0.64)
                    {
                        Cref_Cdac[i] |= BIT(5);
                        temp3[i] = temp3[i] - 0.64;
                    }
                    if(temp3[i]>0.32)
                    {
                        Cref_Cdac[i] |= BIT(4);
                        temp3[i] = temp3[i] - 0.32;
                    }
                    if(temp3[i]>0.16)
                    {
                        Cref_Cdac[i] |= BIT(3);
                        temp3[i] = temp3[i] - 0.16;
                    }
                    if(temp3[i]>0.08)
                    {
                        Cref_Cdac[i] |= BIT(2);
                        temp3[i] = temp3[i] - 0.08;
                    }
                    if(temp3[i]>0.04)
                    {
                        Cref_Cdac[i] |= BIT(1);
                        temp3[i] = temp3[i] - 0.04;
                    }
                    if(temp3[i]>0.025)
                    {
                        Cref_Cdac[i] |= BIT(0);               
                    }                                                
                    afe2_tk_ref_calibration_set(tk_g_ainp_channel[i]-0x10,Cref_Cdac[i]);       //配置CREF_DACx            
                    hal_tk_afe_rawdata_old[i] = hal_tk_afe_rawdata[i];    
                    auto_calibration_flag[i] = 1;     
                    ff_flag  = 1;             
                }
            }
        }
    }
}
/**
* @fn void hal_tk_sample(void)
* @brief tk sample
* @param None
* @return none
*/
void hal_tk_sample(void)
{
    uint8_t i;
    static int16_t adc_value;
    hal_tk_enable();
    afe2_tk_shield_enable_ctrl(ENABLE);
    for(i = 0; i<TK_CH_NUM;i++)
    {
        afe2_adc_channel_enable_ctrl(tk_g_afe_channel[i],ENABLE);//通道使能选择
        afe2_adc_conversion_start();//开始ADC转换
    #if HAL_TK_INT_ENBALE
        do
        {
            hal_pmu_enter_deep_sleep1();
        }
        while(tk_Finish == 0);
        tk_Finish = 0;
    #else
        while(afe2_adc_result_valid_flag_get(tk_g_afe_channel[i])==RESET);//等待转换完成
    #endif
        afe2_adc_channel_enable_ctrl(tk_g_afe_channel[i],DISABLE);//通道关闭选择
        adc_value = hal_afe_tk_data_get(HAL_AFE_NUM,i);   
        hal_tk_afe_rawdata[i] =  adc_value;                   
    }
    hal_tk_disable();
    afe2_tk_shield_enable_ctrl(DISABLE);
    fw_auto_calibration_proc_tk();
}
/**
* @fn int16_t hal_tk_sample_get_adc_accumulate(uint8_t ch)
* @brief get tk sample accumulate 
* @param ch
* @return ch num sample accumulate
*/
int16_t hal_tk_sample_get_adc_accumulate(uint8_t ch)
{
    return s_tk_adc_accumulate[ch];
}
/**
* @fn uint32_t hal_tk_sample_get_rawdata(uint8_t ch)
* @brief get tk sample rawdata 
* @param None
* @return ch num sample rawdata
*/
int16_t hal_tk_sample_get_rawdata(uint8_t ch)              		  // 获取采样rawdata
{	
	int16_t hal_tk_data;
	if(ch < TK_CH_NUM){
        hal_tk_data = hal_tk_afe_rawdata[ch]-0x2000;
    }
    return hal_tk_data;
}

/**
* @fn void hal_tk_enable(void)
* @brief tk enable
* @param None
* @return ch num
*/
void hal_tk_enable(void)                            //使能tk模块
{
    afe2_pga_enable_ctrl(ENABLE);//open PGA
    afe2_adc_enable_ctrl(ENABLE);//open ADC
//  afe2_dac_enable_ctrl(ENABLE);//open DAC
    afe2_vs_enable_ctrl(ENABLE);//open VS0
}

/**
* @fn void hal_tk_disable(void)
* @brief tk disable
* @param None
* @return ch num
*/
void hal_tk_disable(void)                           //关闭tk模块
{
    afe2_pga_enable_ctrl(DISABLE);//open PGA
    afe2_adc_enable_ctrl(DISABLE);//open ADC
    afe2_dac_enable_ctrl(DISABLE);//open DAC
    afe2_vs_enable_ctrl(DISABLE);//open VS0
}

/**
* @fn void hal_tk_calibration_all(void)
* @brief tk calibration all ch
* @param None
* @return ch num
*/
void hal_tk_calibration_all(void)
{
    uint8_t i;
    //afe2_vs_voltage_set(AFE2_VS_VOLTS_2V8);//VS 输出档位选择
    for(i=0; i<TK_CH_NUM; i++)
    {
        hal_tk_calibration_ch(i);
    }
}

/**
* @fn uint32_t hal_tk_calibration_ch(uint8_t ChannelIdx)
* @brief tk calibration xx ch
* @param None
* @return ch calibration value
*/
uint32_t hal_tk_calibration_ch(uint8_t ChannelIdx)
{
    static uint16_t tk_cap_value;
    //TK自校准
	//================================================================================	
    afe2_adc_channel_enable_ctrl(tk_g_afe_channel[ChannelIdx],ENABLE);//通道使能选择
    afe2_pga_enable_ctrl(ENABLE);//使能PGA
    afe2_adc_enable_ctrl(ENABLE);//使能A/D转换器
    hal_timer_delay_us(10);//1mS	
    afe2_dac_channel_enable_ctrl(tk_g_afe_channel[ChannelIdx],DISABLE);//DAC_ENn(通道n DAC不使能)
    afe2_tk_input_calibration_set((tk_g_ainp_channel[ChannelIdx]-0x10),0x0);//CPIN_DACx(输入电容校准值)
    tk_cap_value = afe2_tk_capacitance_calibration((tk_g_ainp_channel[ChannelIdx]-0x10),tk_g_afe_channel[ChannelIdx]);//TK电容校准流程
    afe2_tk_ref_calibration_set((tk_g_ainp_channel[ChannelIdx]-0x10),tk_cap_value);//配置CREF_DACx
    afe2_dac_channel_enable_ctrl(tk_g_afe_channel[ChannelIdx],ENABLE);//DAC_ENn(通道n DAC重新使能)
    afe2_adc_channel_enable_ctrl(tk_g_afe_channel[ChannelIdx],DISABLE);//通道关闭选择
    Cpin_value[ChannelIdx] = cpin;
    return 0;
}

/**
* @fn void hal_afe_interrput(void)
* @brief afe interrupt fun
* @param None
* @return None.
*/
void hal_tk_interrput(void)            // tk 采样中断调用函数
{
    afe2_interrupt_flag_clear();
    tk_Finish = 1;
}

/**
  * @}
  */

/**
  * @}
  */

///************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
