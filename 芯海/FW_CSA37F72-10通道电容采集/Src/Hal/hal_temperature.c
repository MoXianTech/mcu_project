/**
* @file                hal_temperature.c
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
 * @defgroup 温度传感器
 * @brief 温度传感器
 * @details 
 * @{
 */
//driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_flash.h"
#include "csa37f7x_syscfg.h"

#include "csa37f7x_afe2.h"

//hal
#include "hal_config.h"
#include "hal_afe.h"
#include "hal_timer.h"
#include "hal_temperature.h"

#define HAL_TEMP_SMOOTH_LEN         4   // 温度数据平滑级别

#define TEMPER_PARAMETER_1          ((0.00454)*8192)

//static uint16_t s_TempAdcF = 0;
 
static int16_t hal_temp_rawdata;

static uint16_t hal_temperature_trim[2];

static int16_t s_temp_buff[HAL_TEMP_SMOOTH_LEN];

static uint8_t vs_buff[4] = {20,22,26,28};

static volatile double temper_parameter[2];

/**
  * @fn void hal_temperature_init(void)
  * @brief  Temperature module init
  * @param None
  * @return None
 */
void hal_temperature_init(void)                             // 温度传感器模块初始化
{
    double temp1,temp2;
    hal_temperature_trim[0] = syscfg_temp_trim0_get();
    hal_temperature_trim[1] = syscfg_temp_trim1_get();

    temper_parameter[0] = (double)(vs_buff[HAL_AFE_VS]/TEMPER_PARAMETER_1);

    temp1 = (double)(26.0-(double)vs_buff[HAL_AFE_VS])/2;
    temp2 = (double)(((double)hal_temperature_trim[0]-8192)*26.0/8192.0);   
    temp1 = temp1+temp2;
    temper_parameter[0] = (temp1*8192/vs_buff[HAL_AFE_VS])+8192;
    temper_parameter[1] = (double)((double)hal_temperature_trim[1]/10.0-2731.5);

}
/**
  * @fn short hal_get_temp_rel(void)
  * @brief  Get temperature
  * @param None
  * @return temperature * 100
 */

int16_t hal_get_temperature_adc_value(void)                // 获取原始温度传感器的温度数据
{
    double temp1;
    int16_t TempSensor_t;
    afe2_pga_bypass_set(AFE2_AIN_Channel_9,AFE2_PGA_BYPASS_BOTH);
    afe2_adc_channel_config(AFE2_AIN_Channel_9,AFE2_AINP_SOURCE_ITSP,AFE2_AINN_SOURCE_ITSN);//ÉèÖÃP N¶Ë½ÓÎÂ¶È´«¸ÐÆ÷
    hal_temp_rawdata = hal_afe_temperature_sample(AFE2_AIN_Channel_9);
    afe2_pga_bypass_set(AFE2_AIN_Channel_9,AFE2_PGA_BYPASS_NONE); 
 
    temp1 = ((double)hal_temp_rawdata-temper_parameter[0])*vs_buff[HAL_AFE_VS];
    TempSensor_t = (temp1/TEMPER_PARAMETER_1)+temper_parameter[1];
    return TempSensor_t;
}


/**
* @fn int16_t hal_temperature_get_rawdata(void)
* @brief get  hal_temp_rawdata data
* @param none
* @return sample data
*/
int16_t hal_temperature_get_rawdata(void)
{
    return hal_temp_rawdata;
}

/**
  * @fn short hal_get_temp(void)
  * @brief  Get average temperature
  * @param None
  * @return temperature * 100
 */
int16_t hal_get_temprature(void)                           // 获取平滑滤波后的温度数据
{
    int i;
    int sum = 0;
    for(i = HAL_TEMP_SMOOTH_LEN - 1; i > 0; i--)            // n次温度数据累计
    {
        s_temp_buff[i] = s_temp_buff[i - 1];
        sum += s_temp_buff[i];
    }
    s_temp_buff[0] = hal_get_temperature_adc_value();       // 获取最新温度值
    sum += s_temp_buff[0];
    return sum / HAL_TEMP_SMOOTH_LEN;                       // 返回平均值
}

int16_t get_temperature_rawdata(void)
{
    return hal_temp_rawdata;
}

int16_t get_temperature_trim0(void)
{
    return hal_temperature_trim[0];
}

int16_t get_temperature_trim1(void)
{
    return hal_temperature_trim[1];
}


/**
  * @}
  */

/**
  * @}
  */
