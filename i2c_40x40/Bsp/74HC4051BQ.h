#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"

#define SENSOR_POS_X 40
#define SENSOR_POS_Y 40

typedef struct {
    uint8_t printf_buffer[128];
    uint16_t adc_raw_value[SENSOR_POS_X][SENSOR_POS_Y];
    uint32_t adc_total_vcc_value[SENSOR_POS_Y];
    uint8_t adc_cali_value[SENSOR_POS_X][SENSOR_POS_Y];
    uint8_t x_max;
    uint8_t y_max;
} process_handle_t;

void rs2251Delay(uint32_t num);
void adc_value_read(process_handle_t *process_handle, uint8_t count_y);
void rs2251IoInit(void);
void select_x_control(uint8_t x_value);
void select_y_control(process_handle_t *process_handle, uint8_t y_value);
void adc_calculation_calibration_once(process_handle_t *process_handle, bool enable_flag);

#endif
