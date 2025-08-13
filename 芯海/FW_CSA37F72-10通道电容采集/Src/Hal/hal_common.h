/**
* @file                hal_common.c
* @brief
* @author              
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include <stdint.h>                   /* Include standard types */
#include <string.h>

void hal_common_get_chip_ID(uint8_t *id_data, uint8_t num);
void hal_common_ic_soft_reset(void);
unsigned int calc_crc32(const void *buf, unsigned int size);

#endif /*__HAL_COMMON_H_ */

