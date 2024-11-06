#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32f4xx.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
//#include "flash.h"
#include "main.h"
#include "resi_math_lib.h"
#include "74hc595.h"

typedef struct {
    uint16_t adc_sensor_value[POS_X][POS_Y];
    uint16_t adc_resi_value[POS_Y];
    uint16_t resi_cali_value[POS_X][POS_Y];
    uint8_t display_matrix[POS_X][POS_Y];
    uint8_t x_max;
    uint8_t y_max;
} process_handle_t;

typedef enum {
    SINGLE_VCC_READ,
    TOTAL_VCC_READ,
    SINGLE_RESI_READ
} ADC_MODE_TYPE_T;


void adc_value_read(process_handle_t *process_handle, uint8_t count_y, ADC_MODE_TYPE_T mode_type);
void rs2251_io_init(void);
void select_x_control(uint8_t x_value);
void select_y_control(process_handle_t *process_handle, uint8_t y_value);
void cal_resi_value(process_handle_t *process_handle, uint8_t y_value);

#endif
