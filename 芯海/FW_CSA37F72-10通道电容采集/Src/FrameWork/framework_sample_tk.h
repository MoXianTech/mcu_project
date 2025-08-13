/**
* @file                framework_sample_tk.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.6.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef __FRAMEWORK_SAMPLE_TK_H_
#define __FRAMEWORK_SAMPLE_TK_H_

#include <stdint.h>                   /* Include standard types */

//config
#include "hal_config.h"

void fw_sample_tk_init(void);                                  // fw内部使用：tk模块初始化
void fw_tk_sample_proc(void);
int16_t get_fw_tk_rawdata(uint8_t ch) ;

#endif



