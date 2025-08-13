/**
* @file                framework_sample.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup Framework层
  * @{
  */
/**
 * @defgroup 采样模块
 * @brief 同时采样force_touch,tk数据
 * @details 
 * @{
 */
//hal
#include "hal_afe.h"
#include "hal_tk.h"
#include "hal_pmu.h"

//framework
#include "framework_sample.h"
#include "framework_debug.h"
#include "framework_pmu.h"
#include "framework_reg.h"
#include "framework_sample_tk.h"
#include "framework_sample.h"


#define ABS(x)  (((x)>0) ? (x) : (0-(x)))
#define ALL_CH_NUM     (TK_CH_NUM+CH_NUM)

static uint16_t all_g_afe_channel[ALL_CH_NUM];
static uint8_t all_g_ainn_channel[ALL_CH_NUM];
static uint8_t all_g_ainp_channel[ALL_CH_NUM];
void fw_auto_calibration_proc(void);
/**
* @fn void fw_sample_init_all(void)
* @brief afe init
* @param None
* @return None.
*/
volatile uint8_t vg_all_afe_Finish = 0;

/**
* @fn void fw_all_afe_interrput(void)
* @brief afe interrupt fun
* @param None
* @return None.
*/
void fw_all_afe_interrput(void)            // 采样中断调用函数
{
    afe2_interrupt_flag_clear();
    vg_all_afe_Finish = 1;
}


void fw_sample_init_all(void)                               // afe 初始化
{
    uint8_t i;

    hal_tk_init();
    hal_afe_init();
    for(i=0;i<TK_CH_NUM;i++)
    {
        all_g_afe_channel[i] = tk_g_afe_channel[i];
        all_g_ainn_channel[i] = tk_g_ainn_channel[i];
        all_g_ainp_channel[i] = tk_g_ainp_channel[i];
    }

    for(i=0;i<CH_NUM;i++)
    {
        all_g_afe_channel[TK_CH_NUM + i] = g_afe_channel[i];
        all_g_ainn_channel[TK_CH_NUM+ i] = g_ainn_channel[i];
        all_g_ainp_channel[TK_CH_NUM+ i] = g_ainp_channel[i];
    }
}

#define SOFT_AVERAGE_NUM   8

void fw_sample_proc_all(void)
{
    uint8_t i;
    uint16_t temp_adc;
    int16_t temp_data;
    uint8_t j = 0;
    int32_t  sum[ALL_CH_NUM]={0};

    for(j =0;j<SOFT_AVERAGE_NUM;j++)
    {
        hal_tk_enable();
        afe2_tk_shield_enable_ctrl(ENABLE);
        for(i = 0; i<ALL_CH_NUM;i++)
        {
            afe2_adc_channel_enable_ctrl(all_g_afe_channel[i],ENABLE);//通道使能选择
            afe2_adc_conversion_start();//开始ADC转换
        #if HAL_TK_INT_ENBALE
            do
            {
                if(PMU_ACTIVE_MODE == fw_pmu_get_run_mode())
                {
                    hal_pmu_enter_sleep();
                }
                else
                {
                    hal_pmu_enter_deep_sleep1();
                }
            }
            while(vg_all_afe_Finish == 0);
            vg_all_afe_Finish = 0;
        #else
            while(afe2_adc_result_valid_flag_get(tk_g_afe_channel[i])==RESET);//等待转换完成
        #endif

             temp_adc = afe2_adc_conversion_value_get(all_g_afe_channel[i]);//获取码值

             temp_data = temp_adc>>2;
             sum[i] += temp_data;

            // if(i<TK_CH_NUM)
            // {
            //     temp = afe2_adc_conversion_value_get(all_g_afe_channel[i]);//获取码值
            //     hal_tk_afe_rawdata[i] = temp>>2;
            //     sum[i] += hal_tk_afe_rawdata[i] ;
            // }
            // else
            // {
            //     temp = afe2_adc_conversion_value_get(all_g_afe_channel[i]);//获取码值
            //     hal_afe_rawdata[i-TK_CH_NUM] = temp>>2;
            // }
            afe2_adc_channel_enable_ctrl(all_g_afe_channel[i],DISABLE);//通道关闭选择
        }
        hal_tk_disable();
        afe2_tk_shield_enable_ctrl(DISABLE);
    }

    for(i=0;i<ALL_CH_NUM;i++)
    {
        if(i<TK_CH_NUM)
        {
            hal_tk_afe_rawdata[i] = sum[i]/SOFT_AVERAGE_NUM;
        }
        else
        {
            hal_afe_rawdata[i-TK_CH_NUM] = sum[i]/SOFT_AVERAGE_NUM;
        }
    }


}

void fw_sample_proc_singel(uint8_t ch)
{
    //  uint8_t i;
    uint16_t temp_adc;
    int16_t temp_data;
    uint8_t j = 0;
    int32_t  sum=0;

    for(j =0;j<SOFT_AVERAGE_NUM;j++)
    {
        hal_tk_enable();
        afe2_tk_shield_enable_ctrl(ENABLE);
        // for(i = 0; i<ALL_CH_NUM;i++)
        {
            afe2_adc_channel_enable_ctrl(all_g_afe_channel[ch],ENABLE);//通道使能选择
            afe2_adc_conversion_start();//开始ADC转换
        #if HAL_TK_INT_ENBALE
            do
            {
                if(PMU_ACTIVE_MODE == fw_pmu_get_run_mode())
                {
                    hal_pmu_enter_sleep();
                }
                else
                {
                    hal_pmu_enter_deep_sleep1();
                }
            }
            while(vg_all_afe_Finish == 0);
            vg_all_afe_Finish = 0;
        #else
            while(afe2_adc_result_valid_flag_get(tk_g_afe_channel[i])==RESET);//等待转换完成
        #endif

            temp_adc = afe2_adc_conversion_value_get(all_g_afe_channel[ch]);//获取码值
            temp_data = temp_adc>>2;
            sum += temp_data;
            afe2_adc_channel_enable_ctrl(all_g_afe_channel[ch],DISABLE);//通道关闭选择
        }
        hal_tk_disable();
        afe2_tk_shield_enable_ctrl(DISABLE);
    }

    // for(i=0;i<ALL_CH_NUM;i++)
    {
        if(ch<TK_CH_NUM)
        {
            hal_tk_afe_rawdata[ch] = sum/SOFT_AVERAGE_NUM;
        }
        else
        {
            hal_afe_rawdata[ch-TK_CH_NUM] = sum/SOFT_AVERAGE_NUM;
        }
    }
}
extern uint32_t hal_tk_calibration_ch_test(uint8_t ChannelIdx);
static uint32_t  bak_adc[ALL_CH_NUM]={0};
int16_t calibrate_offset[ALL_CH_NUM]={0};
 uint32_t calibrate_cnt=0;
 int16_t calibrate_trg_cnt[ALL_CH_NUM] = {0};
// static uint8_t is_stable_raw(int16_t )
static void fw_auto_calibration_proc(void)
{
//    int i=0;

//    for(i=0;i<ALL_CH_NUM;i++)
//    {
//        if(i<TK_CH_NUM)
//        {
////            if(ABS ((int16_t)(hal_tk_afe_rawdata[i] - 0x2000))> 5000 )
////            {
////                uint8_t isStableFlag=0;
////                calibrate_trg_cnt[i]++;

////                if(calibrate_trg_cnt[i]>4)
////                {
////                    calibrate_trg_cnt[i] = 0;
////                    if(i<4)
////                    {
////                        isStableFlag = Alg_IEDGetStableFlag(i);
////                    }
////                    else
////                    {
////                        isStableFlag = Alg_TouchGetStableFlag(i-4);
////                    }
////                    if(isStableFlag )
////                    {
////                        bak_adc[i] = hal_tk_afe_rawdata[i];
////                        calibrate_cnt++;
////                        hal_tk_calibration_ch(i);
////                        fw_sample_proc_singel(i);

////                        calibrate_offset[i] +=(int16_t) (bak_adc[i]-hal_tk_afe_rawdata[i]);
////                    }
////                }
////            }
////            else
////            {
////                calibrate_trg_cnt[i]=0;
////            }
//        }
//        else
//        {
//            if(ABS ((int16_t)(hal_afe_rawdata[i-TK_CH_NUM] - 0x2000) )> 4000 )
//            {
//                calibrate_trg_cnt[i]++; 
//                if(calibrate_trg_cnt[i] > 4)
//                {
////                    uint8_t isStableFlag=Alg_ForceGetStableFlag(i-TK_CH_NUM);
//                    calibrate_trg_cnt[i]=0;
//                    if(isStableFlag )
//                    {
//                        calibrate_trg_cnt[i]=0;
//                        calibrate_cnt++;
//                        bak_adc[i] = hal_afe_rawdata[i-TK_CH_NUM];

//                        afe2_pga_enable_ctrl(ENABLE);                   //open PGA
//                        afe2_vs_enable_ctrl(ENABLE);                    //open  VS
//                        afe2_adc_enable_ctrl(ENABLE);                   //open ADC
//                        //DAC校正使能的通道
//                        hal_afe_calibration_ch(g_afe_channel, i-TK_CH_NUM);
//                        afe2_vs_enable_ctrl(DISABLE);

//                        fw_sample_proc_singel(i);
//                        calibrate_offset[i] += (int16_t)( bak_adc[i] - hal_afe_rawdata[i-TK_CH_NUM]);
//                    }
//                }
//            }
//            else
//            {
//                calibrate_trg_cnt[i]=0;
//            }
//        }
//    }

}

/**
  * @}
  */

/**
  * @}
  */