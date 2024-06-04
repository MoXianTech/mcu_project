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

uint8_t math_max = 35;

void adc_calculation_calibration_once(process_handle_t *process_handle, uint8_t *serial_buffer, bool enable)
{
    uint8_t count_x = 0, count_y = 0;	
    int32_t adc_value_2 = 0;
    uint8_t (*serial_value)[POS_Y] = (uint8_t (*)[POS_Y])serial_buffer;
    uint8_t filter_value = 0;

    for (count_y = 0; count_y < POS_Y; count_y ++)
    {
        for (count_x = 0; count_x < POS_X; count_x ++)
        {
            if (process_handle->adc_raw_value[count_x][count_y] < math_max) ;
						else
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
                serial_value[count_x][count_y] = 0;
            } else {
                if (enable)
                {

                    adc_value_2 = 254 * adc_value_2 / (4096 - (process_handle->adc_total_vcc_value[count_y] - adc_value_2));
//                    if (adc_value_2 < 10) filter_value = 1;
//                    if (adc_value_2 < 30) filter_value = 3;
//                    if (adc_value_2 < 50) filter_value = 5;
//                    if (adc_value_2 < 70) filter_value = 7;
//                    if (abs(adc_value_2 - serial_value[count_x][count_y]) > filter_value)
//                    {
                        serial_value[count_x][count_y] = adc_value_2;
//                    }
                } else {
                    serial_value[count_x][count_y] = process_handle->adc_raw_value[count_x][count_y] / 16;
                }
            }

        }
    }

    memset(process_handle->adc_total_vcc_value, 0x00, 4 * POS_Y);
}



