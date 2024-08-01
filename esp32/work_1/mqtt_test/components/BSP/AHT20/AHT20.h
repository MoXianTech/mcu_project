#ifndef __AHT20_H__
#define __AHT20_H__

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "iic.h"


#define ATH20_ADDR  0x38

#define ATH20_INIT   0xBE
#define	ATH20_SOFT_RESET	0xBA
#define	ATH20_START_TEST  0xAC
#define ATH20_START_STATE 0x71

void ATH20_init(i2c_obj_t self);
void ATH20_read_data(uint16_t *temp_value, uint16_t *humi_value);

#endif
