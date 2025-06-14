#ifndef __RESI_MATH_LIB_H__
#define __RESI_MATH_LIB_H__

#include "stdint.h"



typedef enum {
    SCAN_HIGH_LEVEL_DOUBLE_SWEEP,
    SCAN_LOW_LEVEL_DOUBLE_SWEEP,
    SCAN_HIGH_LEVEL_SINGLE_SWEEP,
    SCAN_LOW_LEVEL_SINGLE_SWEEP
} SCAN_LEVEL_T;

typedef enum {
    SINGLE_BYTE,
    DOUBLE_BYTE
} DATA_BIT_SIZE_T;

typedef enum {
    RESI_BACKWARDS,
    RESI_SCALE
} MATH_RESI_DISPLAY_TYPE_T;

typedef struct {
    void *sensor_adc_value;
    void *resi_adc_value;
    void *math_resi_value;
    uint16_t value_count;
    DATA_BIT_SIZE_T data_bit_size;
    SCAN_LEVEL_T scan_level;
} math_resi_cal_t;

void math_resi_cali(math_resi_cal_t *math_resi_cal);

void math_display_resi(uint16_t *math_resi_buffer,
        void *display_buffer,
        uint16_t buffer_size,
        DATA_BIT_SIZE_T data_bit_size,
        MATH_RESI_DISPLAY_TYPE_T type);

void math_resi_init(uint16_t min_display_value,
        uint16_t max_display_value,
        uint16_t max_value,
        uint16_t adc_max_value);

#endif
