/**
* @file                framework_system.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef __FRAMEWORK_SYSTEM_H_
#define __FRAMEWORK_SYSTEM_H_

#include <stdint.h>                   /* Include standard types */

#define BIT(X) ((uint32_t)(1)<<(X))

typedef struct
{
    int flag;
    int version;
    int ch_num;

} ft_system_param_t;

typedef struct
{
    int flag;
    int version;
    int crc32;
    int ch_num;
    int calibration_factor[16];
    int temperature_corr;
} ft_factory_param_t;

typedef struct
{
    int flag;
    int version;
    int crc32;
    unsigned short key_down_threshold[16];
    unsigned short key_up_threshold[16];
} ft_user_param_t;

extern ft_system_param_t g_system_param;
extern ft_factory_param_t g_factory_param;
extern ft_user_param_t g_user_param;



void fw_system_init(void);                                          // fw内部使用：system模块初始化
void fw_system_app_enable_set(uint16_t enable);                     // fw内部使用：设置ap功能使能信息
uint16_t fw_system_app_enable_get(void);                            // ap可使用：获取允许ap功能使能信息

int16_t fw_system_get_temperature(void);                            // ap可使用：获取温度传感器温度信息（实际温度值*100）
void fw_system_check(void);                                         // fw内部使用：周期性检查系统异常
uint32_t fw_system_crc32(const void *buf, uint16_t size);           // ap可使用：计算指定数据的crc32校验码
uint16_t fw_system_crc16(const void *buf, uint16_t size);           // ap可使用：计算指定数据的crc16校验码

void fw_system_time_intertupt(void);                                // fw内部使用：system模块需要time中断的处理函数
uint32_t fw_system_get_systick_time(void);                          // ap可使用：获取系统计时数值（单位：ms）

void ft_config_init(void);                                          // fw内部使用：校准数据模块初始化
int ft_get_system_config(ft_system_param_t *p_system_param);        // fw内部使用：获取system参数的flash数据
int ft_set_system_config(ft_system_param_t *p_system_param);        // fw内部使用：设置system参数的flash数据
int ft_get_factory_config(ft_factory_param_t *p_factory_param);     // fw内部使用：获取factory参数的flash数据
int ft_set_factory_config(ft_factory_param_t *p_factory_param);     // fw内部使用：设置factory参数的flash数据
int ft_get_user_config(ft_user_param_t *p_user_param);              // fw内部使用：获取user参数的flash数据
int ft_set_user_config(ft_user_param_t *p_user_param);              // fw内部使用：设置user参数的flash数据
uint32_t fw_system_get_calibration_factor(uint8_t ch);              // ap可使用：获取指定通道的factor校准参数
void fw_system_delay_ms(int16_t time_ms);                           // ap可使用：设置程序delay时间（ms）
void fw_system_delay_us(int16_t time_us);                           // ap可使用：设置程序delay时间（us）
void fw_system_set_iic_flag(void);                                  // fw内部使用：设置iic通讯状态
uint8_t fw_system_get_iic_flag(void);                               // ap可使用：获取当前是否存在iic通讯

#endif



