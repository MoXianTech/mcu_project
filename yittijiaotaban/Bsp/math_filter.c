/*************************************************************************
  > File Name: math_filter.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: 2022/9/8 17:34:21
 ************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "math_filter.h"
#include "main.h"
#include "74hc4051bq.h"

uint8_t math_max = 0;

void adc_calculation_calibration_once(process_handle_t *process_handle, bool enable)
{
    uint8_t count_x = 0, count_y = 0;	
    uint32_t adc_value_2 = 0;
    for (count_y = 0; count_y < POS_Y; count_y ++)
    {
        for (count_x = 0; count_x < POS_X; count_x ++)
        {
            if (process_handle->adc_raw_value[count_x][count_y] < 10) continue;
            process_handle->adc_total_vcc_value[count_y] += process_handle->adc_raw_value[count_x][count_y];
        }

        if (process_handle->adc_total_vcc_value[count_y] >=  4096)
            process_handle->adc_total_vcc_value[count_y] = 4096; 
    }

    for (count_y = 0; count_y < POS_Y; count_y ++)
    {
        for (count_x = 0; count_x < POS_X; count_x ++)
        {
            adc_value_2 = process_handle->adc_raw_value[count_x][count_y];
            if (adc_value_2 < math_max) {
                process_handle->adc_cali_value[count_x][count_y] = 0;
            } else {
                if (enable)
                {
                    process_handle->adc_cali_value[count_x][count_y]
                        = 254 * adc_value_2 / (4096 - (process_handle->adc_total_vcc_value[count_y] - adc_value_2));

                    if (abs((int16_t)process_handle->adc_cali_value[count_x][count_y] - process_handle->adc_cali_value_record[count_x][count_y]) < math_max)
                        process_handle->adc_cali_value[count_x][count_y] = process_handle->adc_cali_value_record[count_x][count_y];
                    if (process_handle->adc_cali_value[count_x][count_y] < math_max)
                        process_handle->adc_cali_value[count_x][count_y] = 0;
                } else {
									  if (abs((int16_t)process_handle->adc_cali_value[count_x][count_y] - process_handle->adc_cali_value_record[count_x][count_y]) < math_max)
                        process_handle->adc_cali_value[count_x][count_y] = process_handle->adc_cali_value_record[count_x][count_y];
                    process_handle->adc_cali_value[count_x][count_y] = process_handle->adc_raw_value[count_x][count_y] / 16;
                }
            }

        }
    }

    memcpy(process_handle->adc_cali_value_record, process_handle->adc_cali_value, POS_X * POS_Y);
    memset(process_handle->adc_total_vcc_value, 0x00, 4 * POS_Y);
}

