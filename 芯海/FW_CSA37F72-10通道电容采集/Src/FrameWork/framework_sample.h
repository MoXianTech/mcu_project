/**
* @file                framework_sample.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef __FRAMEWORK_SAMPLE_H_
#define __FRAMEWORK_SAMPLE_H_

#include <stdint.h>                   /* Include standard types */


//config
#include "hal_config.h"

int16_t fw_sample_get_rawdata(uint8_t num);                         // ap可使用：获取指定通道的采样值
uint16_t fw_sample_get_sensor_connect_status(uint8_t ch_id);        // ap可使用：获取指定通道的sensor通断状态

void fw_sample_init(void);                                  // fw内部使用：sample模块初始化
void fw_sample_noise_cal_en(uint16_t count);                // fw内部使用：启动noise计算功能
static void fw_sample_noise_cal(void);                      // fw内部使用：noise计算函数
uint8_t fw_sample_noise_get(int16_t *noise_peak, int32_t *noise_dev_sum, uint8_t num);      // fw内部使用：获取noise计算结果
uint8_t fw_sample_get_restart_status(void);
void fw_sample_proc(void);                                  // fw内部使用：sample模块处理函数

#endif



