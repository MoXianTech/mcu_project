#ifndef __RESI_MATH_LIB_H__
#define __RESI_MATH_LIB_H__

#include "stdint.h"

typedef struct {
    uint16_t *sensor_adc_value;
    uint16_t *resi_adc_value;
    uint16_t resi_value;
    uint16_t adc_max_value;
    uint16_t math_number;
} math_resi_cal_t;

typedef enum {
    SCAN_HIGH_LEVEL,
    SCAN_LOW_LEVEL
} SCAN_LEVEL_T;

void math_resi_cali_once(math_resi_cal_t *math_resi_cal,
                         uint16_t *math_resi_buffer,
                         uint16_t math_resi_buffer_size,
                         SCAN_LEVEL_T scan_level);

void math_display_resi(uint16_t *math_resi_buffer,
                       uint8_t *display_buffer,
                       uint16_t buffer_size);

void math_resi_init(uint16_t max_display_resi_cal,
                    uint16_t min_display_resi_cal,
                    uint16_t max_display);

#endif
