#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"
#include "main.h"

typedef struct {
    uint8_t printf_buffer[CACHE_BUFFER_SIZE];
    uint8_t recv_cache[CACHE_BUFFER_SIZE];
    uint16_t adc_raw_value[POS_X][POS_Y];
    uint32_t adc_total_vcc_value[POS_Y];
    uint8_t adc_cali_value[POS_X][POS_Y];
    uint8_t adc_cali_value_record[POS_X][POS_Y];
    uint8_t x_max;
    uint8_t y_max;
} process_handle_t;


void adc_value_read(process_handle_t *process_handle, uint8_t count_y);
void rs2251IoInit(void);
void select_x_control(uint8_t x_value);
void select_y_control(process_handle_t *process_handle, uint8_t y_value);

#endif
