/**
* @file                hal_pmu.c
* @brief
* @author              
* @version             V0.1.0
* @date                2018.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_PMU_H
#define HAL_PMU_H

#include <stdint.h>                   /* Include standard types */

void hal_pmu_init(void);
void hal_pmu_enter_sleep(void);
void hal_pmu_enter_deep_sleep1(void);
void hal_pmu_enter_deep_sleep2(void);

#endif /* HAL_PMU_H */
