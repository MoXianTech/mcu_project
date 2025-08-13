/**
* @file                hal_temperature.c
* @brief
* @author              
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef HAL_TEMPERATURE_H
#define HAL_TEMPERATURE_H

#include <stdint.h>                   /* Include standard types */

void hal_temperature_init(void);
int16_t hal_get_temprature(void);
int16_t hal_get_temperature_adc_value(void);
int16_t hal_temperature_get_rawdata(void);
int16_t get_temperature_rawdata(void);
int16_t get_temperature_trim0(void);
int16_t get_temperature_trim1(void);

#endif /* HAL_TEMPERATURE_H */







