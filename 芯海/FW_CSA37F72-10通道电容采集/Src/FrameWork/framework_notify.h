/**
* @file                framework_notify.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef FRAMEWORK_NOTIFY_H
#define FRAMEWORK_NOTIFY_H

#include <stdint.h>                   /* Include standard types */

typedef enum
{
    NOTICY_KEY_0 = 0,
    NOTICY_KEY_1 = 1,
    NOTICY_KEY_2 = 2,
    NOTICY_KEY_3 = 3,
    NOTICY_KEY_4 = 4,
    NOTICY_KEY_5 = 5,
    NOTICY_KEY_6 = 6,
    NOTICY_KEY_7 = 7,
} fw_noticy_key_num_t;

typedef enum
{
    NOTICY_KEY_UP = 0,
    NOTICY_KEY_DOWN = 1,
} fw_noticy_key_stauts_t;

void fw_notify_init(void);                          // fw内部使用： notify模块初始化
void fw_notify_int_out_enable(void);                // fw内部使用： 使能io中断输出功能
void fw_notify_int_out_disable(void);               // fw内部使用： 关闭io中断输出功能
void fw_notify_key_out_enable(void);                // fw内部使用： 使能key io输出功能
void fw_notify_key_out_disable(void);               // fw内部使用： 关闭key io输出功能

void fw_notify_int_out_once(void);                  // ap可使用： 通知noticy模块输出1次INT 中断

void fw_notify_int_out_clear(void);                 // fw内部使用：清除int输出标记
void fw_notify_all_key_out_clear(void);             // fw内部使用：清除key io输出标记
void fw_notify_set_key_status(fw_noticy_key_num_t key_num, fw_noticy_key_stauts_t key_status);  // ap可使用：设置key按压状态
void fw_notify_set_int_in_status(uint8_t status);   // fw内部使用：设置int输入引脚电平状态

uint8_t fw_notify_get_int_in_status(void);          // ap可使用：获取int 输入引脚是否产生跳变标记
uint8_t fw_notify_get_int_in_pin_status(void);      // ap可使用：获取int 输入引脚的电平状态
void fw_notify_set_motor_trig_once(void);           // ap可使用：通知noticy模块输出1次马达震动

void fw_notify_motor_trig_enable(void);             // fw内部使用：使能马达输出模块
void fw_notify_motor_trig_disable(void);            // fw内部使用：关闭马达输出模块
void fw_notify_proc(void);                          // fw内部使用：noticy模块处理函数


void fw_notify_set_work_status(uint8_t status);
uint8_t fw_notify_get_work_status(void);
#endif




