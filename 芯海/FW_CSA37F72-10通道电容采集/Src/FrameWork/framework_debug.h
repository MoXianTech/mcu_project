/**
* @file                framework_debug.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef __FRAMEWORK_DEBUG_H_
#define __FRAMEWORK_DEBUG_H_

#include <stdint.h>                   /* Include standard types */
#include <stdio.h>

extern char fw_debug_iic_printf_receive[];                          // fw内部使用

void fw_debug_init(void);                                           // fw内部使用：fw debug功能初始化
void fw_printf_array_int16(int16_t *array, uint16_t num);           //
void fw_printf_array_hex32(uint32_t *array, uint16_t num);
void fw_printf_out_enable(void);                                    // fw内部使用
void fw_printf_out_disable(void);                                   // fw内部使用

void fw_printf_iic(uint16_t num);                                   // ap可使用：写入打印的数据个数

uint16_t fw_printf_iic_get_data_num(void);                          // fw内部使用：获取iic打印缓存数据个数
uint8_t fw_printf_iic_get_data(uint16_t num);                       // fw内部使用：获取指定位置的iic打印缓存数值
void fw_printf_iic_out_enable(void);                                // fw内部使用：iic 打印功能使能
void fw_printf_iic_out_disable(void);                               // fw内部使用：iic 打印功能关闭
void fw_sample_print(void);
void fw_sample_data_print(void);
void fw_sample_data_key_set(uint8_t state1,uint8_t state2,uint8_t state3);
#endif



