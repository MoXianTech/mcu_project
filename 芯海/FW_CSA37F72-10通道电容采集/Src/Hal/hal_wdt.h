/**
* @file                hal_wdt.c
* @brief
* @author              
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef HAL_WDT_H
#define HAL_WDT_H

#include <stdint.h>                   /* Include standard types */

void hal_wdt_init(void);
void hal_wdt_refresh(void);
void hal_wdt_init_longest_reset_time(void);

#endif /*HAL_WDT_H */







