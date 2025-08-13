/**
* @file                framework_pmu.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef FRAMEWORK_PMU_H
#define FRAMEWORK_PMU_H

#include <stdint.h>                   /* Include standard types */

typedef enum
{
	PMU_ACTIVE_MODE = 0,     //IIC
	PMU_STANDARD_MODE = 1,   //100hz standard
	PMU_LOWPOWER_MODE = 2,   //10hz Green
	PMU_DEEP_LOWPOWER_MODE = 3,  //5hz standby
	PMU_SLEEP_MODE = 4,    //1hz  //sleep
} fw_pmu_run_mode_t;

void fw_pmu_init(void);                                     // fw内部使用：pmu模块初始化函数
void fw_pmu_timer_interrput(void);                          // fw内部使用：

void fw_pmu_set_run_mode(fw_pmu_run_mode_t mode);           // ap可使用：设置系统运行模式
fw_pmu_run_mode_t fw_pmu_get_run_mode(void) ;               // ap可使用：获取当前系统运行的模式
uint16_t fw_pmu_get_period_time(void);                      // ap可使用，获取当前系统运行周期时间
void fw_pmu_set_exit_waiting_level(uint8_t exit_level);     // fw内部使用：设置空闲等待允许退出的等级
void fw_pmu_set_sleep_cmd(uint8_t cmd);                     // fw内部使用：设置是否允许进入sleep模式
void fw_pmu_proc(void);                                     // fw内部使用：pmu模块处理函数

#endif

