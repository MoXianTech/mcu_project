/**
* @file                hal_gpio.c
* @brief
* @author
* @version             V0.1.0
* @date                2018.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>                   /* Include standard types */
#include "csa37f7x_gpio.h"
//config
#include "hal_config.h"

void hal_gpio_init(void);
void hal_gpio_int_out_init(void);
void hal_gpio_key_init(void);
void hal_gpio_int_out_high(void);
void hal_gpio_int_out_low(void);
void hal_gpio_set_key_pin(uint16_t gpio, uint8_t status);
void hal_gpio_int_in_init(void);
uint8_t hal_gpio_get_int_in_pin(void);
void hal_gpio_motor_init(void);
void hal_gpio_set_motor0_high(void);
void hal_gpio_set_motor0_low(void);
void hal_gpio_set_motor1_high(void);
void hal_gpio_set_motor1_low(void);
void hal_gpio_get_int_in_int_state_clear(void);
#endif







