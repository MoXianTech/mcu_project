/**
* @file                user_config.h
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USER_CONFIG_H
#define USER_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "csa37f7x.h"
#include "csa37f7x_afe2.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_syscfg.h"



/* PMU config ----------------------------------------------------------------*/
#define PMU_INTO_LOWPOWER_TIME                      200     //200*10ms    从active 模式 进入 lowpower 模式的间隔时间
#define PMU_AUTO_LOWPOWER_ENABLE                    0       //1: enable  / 0:disable        是否允许自动进入lowpower 模式
#define PMU_AUTO_DEEP_LOWPOWER_ENABLE               1       //1: enable  / 0:disable        是否允许自动进入lowpower 模式

#define PMU_INTO_SLEEP_TIME                         200     //100*10ms    从active 模式 进入 sleep 模式的间隔时间

#define PMU_ACTIVE_MODE_PERIOD_TIME                 10      //10*1ms     active 模式周期
#define PMU_STANDARD_MODE_PERIOD_TIME               10      //10*1ms     standard 模式周期
#define PMU_LOWPOWER_MODE_PERIOD_TIME               100     //10*1ms     lowpower 模式周期 green
#define PMU_DEEP_LOWPOWER_MODE_PERIOD_TIME          200     //10*1ms     deep_lowpower 模式周期 standby
#define PMU_SLEEP_MODE_PERIOD_TIME                  1000    //100*1ms    sleep 模式周期

#define PMU_ACTIVE_MODE_POWER_SAVING_LEVEL          1       //active模式周期等待时的省电级别    0:not saving, 1: noraml sleep, 2:deep sleep1,  3:deep sleep2
#define PMU_STANDARD_MODE_POWER_SAVING_LEVEL        3       //active模式周期等待时的省电级别    0:not saving, 1: noraml sleep, 2:deep sleep1,  3:deep sleep2
#define PMU_LOWPOWER_MODE_POWER_SAVING_LEVEL        3       //lowpower模式周期等待时的省电级别  0:not saving, 1: noraml sleep, 2:deep sleep1,  3:deep sleep2
#define PMU_SLEEP_MODE_POWER_SAVING_LEVEL           3       //sleep模式周期等待时的省电级别     0:not saving, 1: noraml sleep, 2:deep sleep1,  3:deep sleep2


/* Notify config -------------------------------------------------------------*/
#define NOTIFY_INT_OUT_FUNCTION_ENABLE              0        // 使能输出IO中断脉冲功能
#define NOTIFY_INT_OUT_DEFAULT_HIGH                 0        // 选择INT输出默认电平 0：低电平，1：高电平
#define NOTIFY_INT_IN_FUNCTION_ENABLE               0        // 使能输入中断功能

#define NOTIFY_KEY_FUNCTION_ENABLE                  1        // 使能输出按键 IO 电平功能

#define NOTIFY_MOTOR_TRIG_ENBALE                    0        // 使能马达驱动功能
#define NOTIFY_MOTOR_INTERVAL_TIME                  18       // 两个trig信号的间隔时间

#define NOTIFY_INT_PULSE_WIDTH                        200      // us 中断脉宽
#define NOTIFY_KEY_NUM                               4        // max 8 按键数量


/* Sample config -------------------------------------------------------------*/
#define FW_SAMPLE_RAWDATA_NUM                       CH_NUM
#define FW_SAMPLE_RAWDATA_MAX                       CH_MAX

#define FW_SAMPLE_RAWDATA_SMOOTH_LEVEL              6            // rawdata 平滑级数
#define FW_SAMPLE_CHECK_SENSOR_TIME                 1000         // 检测sensor通断的时间/周期
#define FW_SAMPLE_RAWDATA_CHANGE_DIR                0            // rawdata 转换符号，调整正负变化时用
#define FW_SAMPLE_SENSOR_GET_WELL_SAMPLE            1            // sensor重新连接正常后是否恢复采样输出 0：不恢复
#define FW_SAMPLE_FILTER_ENABLE                     1            // 是否开启尖峰滤波处理，开启后原始值滞后1帧


/* TK Sample config -------------------------------------------------------------*/
#define FW_TK_SAMPLE_ENABLE	                 	    1          // 是否使能TK采样

#define FW_TK_SAMPLE_RAWDATA_NUM                    TK_CH_NUM
#define FW_TK_SAMPLE_RAWDATA_MAX                    TK_CH_MAX
#define FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL           6          // rawdata 平滑级数
#define FW_TK_SAMPLE_RAWDATA_CHANGE_DIR             0          // rawdata 转换符号，调整正负变化时用
#define FW_TK_SAMPLE_SENSOR_GET_WELL_SAMPLE         1          // sensor重新连接正常后是否恢复采样输出 0：不恢复
#define FW_TK_SAMPLE_FILTER_ENABLE                  1          // 是否开启尖峰滤波处理，开启后原始值滞后1帧

#define FW_TK_FT_RAWDATA_NUM       (FW_TK_SAMPLE_RAWDATA_NUM + FW_SAMPLE_RAWDATA_NUM)


/* Debug config -------------------------------------------------------------*/
#define FW_DEBUG_IIC_NOTIFY                         0           // iic printf后是否输出中断通知主控读取数据 0：不通知，1：通知
#define FW_DEBUG_IIC_BUFFER_NUM                     255         // iic printf 最大长度

#define DEBUG_PRINT_EN                              0           // 是否使能uart打印模块
#define DEBUG_PRINT_DEFAULT_OUTPUT                  1           // UART打印是否默认输出
#define DEBUG_PRINT_IIC_DEFAULT_OUTPUT              1           // 是否默认输出IIC printf 0：不输出、1：输出

#define LINK_DEBUG_ENABLE                            1           //1:LINK Debug 使能，将WDT复位时长设置为最长，避免暂停时进入复位


/* Temp config -------------------------------------------------------------*/
#define TEMPERATURE_TARGET                          2500    // 校准时默认的温度值*100





#endif /*__USER_CONFIG_H */


/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
