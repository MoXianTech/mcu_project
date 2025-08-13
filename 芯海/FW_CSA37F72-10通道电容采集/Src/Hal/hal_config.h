/**
* @file                hal_config.h
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "csa37f7x.h"
#include "csa37f7x_afe2.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_syscfg.h"
#define BIT(X)		((uint32_t)(1)<<(X))
/* tk config-----------------------------------------------------------------*/


#define HAL_TK_INT_ENBALE               (1)                       // TK 采样中断使能
#define TK_CH_NUM                       (4+6)
#define TK_CH_MAX                       (12)
#define TK_CH_START                     (0)
#define TK_CHANNEL_CONFIG               {AFE2_AIN_Channel_0,AFE2_AIN_Channel_1,AFE2_AIN_Channel_2,AFE2_AIN_Channel_3,AFE2_AIN_Channel_4,AFE2_AIN_Channel_5,AFE2_AIN_Channel_6,AFE2_AIN_Channel_7,AFE2_AIN_Channel_8,AFE2_AIN_Channel_9} /*通道9用于检测温度传感器，请优先使用其他通道*/
#define TK_CHANNEL_P_CONFIG             {AFE2_AINP_SOURCE_TK0,AFE2_AINP_SOURCE_TK1,AFE2_AINP_SOURCE_TK2,AFE2_AINP_SOURCE_TK3,AFE2_AINP_SOURCE_TK4,AFE2_AINP_SOURCE_TK5,AFE2_AINP_SOURCE_TK6,AFE2_AINP_SOURCE_TK7,AFE2_AINP_SOURCE_TK8,AFE2_AINP_SOURCE_TK9}
#define TK_CHANNEL_N_CONFIG             {AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING,AFE2_AINP_SOURCE_FLOATING}
#define HAL_TK_AFE_PGA1_GAIN            AFE2_PGA1_GAIN_64//AFE2_PGA1_GAIN_16
#define HAL_TK_AFE_PGA2_GAIN            AFE2_PGA2_GAIN_1//AFE2_PGA2_GAIN_2

/* afe config-----------------------------------------------------------------*/
#define VS_PA0  0x0001
#define VS_PA1  0x0004
#define VS_PA2  0x0010
#define VS_PA3  0x0040
#define VS_PA4  0x0100
#define VS_PA5  0x0400
#define VS_PA6  0x1000
#define VS_PA7  0x4000
#define VS_PA8  0x10000
#define VS_PA9  0x40000

#define HAL_AFE_INT_ENBALE	            1                        // AFE 采样中断使能
#define CH_NUM                         (0)
#define CH_MAX	                       (12)
#define VS_CH_MAX                      (3)
#define AFE_CHANNEL_CONFIG             {AFE2_AIN_Channel_4,AFE2_AIN_Channel_6}    /*通道9用于检测温度传感器，请优先使用其他通道*/
#define CHANNEL_P_CONFIG               {AFE2_AINP_SOURCE_FT4,AFE2_AINP_SOURCE_FT10}// {AFE2_AINP_SOURCE_FT4,AFE2_AINP_SOURCE_FT10}//AFE2_AINP_SOURCE_FT4}
#define CHANNEL_N_CONFIG               {AFE2_AINN_SOURCE_FT6,AFE2_AINP_SOURCE_FT11}// {AFE2_AINN_SOURCE_FT6,AFE2_AINP_SOURCE_FT11}// {AFE2_AINP_SOURCE_FT6}/
#define HAL_AFE_PGA1_GAIN              AFE2_PGA1_GAIN_64            // AFE PGA1
#define HAL_AFE_PGA2_GAIN              AFE2_PGA2_GAIN_4             // AFE PGA2
#define HAL_AFE_VS                     AFE2_VS_VOLTS_2V8            // VS 电压
#define HAL_AFE_NUM                     0                           // 使用的AFE组号

#define BASE_LINE                      (0x2000)


/* gpio config-----------------------------------------------------------------*/
#define HAL_GPIOx_NONE                   0xffff
#define HAL_GPIO_PINx_NONE               0xff

#define HAL_GPIO_INT_OUT_GROUP          HAL_GPIO_PINx_NONE
#define HAL_GPIO_INT_OUT                HAL_GPIO_PINx_NONE          // pin num
#define HAL_GPIO_INT_OUT_PIN            HAL_GPIO_PINx_NONE          // pin

#define HAL_GPIO_INT_IN_GROUP           HAL_GPIO_PINx_NONE
#define HAL_GPIO_INT_IN                 HAL_GPIO_PINx_NONE
#define HAL_GPIO_INT_IN_PIN             BIT(HAL_GPIO_INT_IN)

#define HAL_GPIO_KEY0_GROUP             GPIO_GROUP_A
#define HAL_GPIO_KEY0                   3
#define HAL_GPIO_KEY0_PIN               BIT(HAL_GPIO_KEY0)

#define HAL_GPIO_KEY1_GROUP             HAL_GPIOx_NONE
#define HAL_GPIO_KEY1                   HAL_GPIO_PINx_NONE
#define HAL_GPIO_KEY1_PIN               BIT(HAL_GPIO_KEY1)

#define HAL_GPIO_KEY2_GROUP             HAL_GPIOx_NONE
#define HAL_GPIO_KEY2                   HAL_GPIO_PINx_NONE
#define HAL_GPIO_KEY2_PIN               BIT(HAL_GPIO_KEY2)

#define HAL_GPIO_KEY3_GROUP             HAL_GPIOx_NONE
#define HAL_GPIO_KEY3                   HAL_GPIO_PINx_NONE
#define HAL_GPIO_KEY3_PIN               BIT(HAL_GPIO_KEY3)

#define HAL_GPIO_MOTOR0_GROUP           HAL_GPIO_PINx_NONE
#define HAL_GPIO_MOTOR0                 HAL_GPIO_PINx_NONE
#define HAL_GPIO_MOTOR0_PIN             BIT(HAL_GPIO_MOTOR0)

#define HAL_GPIO_MOTOR1_GROUP           HAL_GPIO_PINx_NONE
#define HAL_GPIO_MOTOR1                 HAL_GPIO_PINx_NONE
#define HAL_GPIO_MOTOR1_PIN             BIT(HAL_GPIO_MOTOR1)


#endif /* HAL_CONFIG_H */


/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/

