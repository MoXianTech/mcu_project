/*************************************************************************
	> File Name: math_sensor.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/10/26 16:34:28
 ************************************************************************/

#include "math_sensor.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>

void adc_calculation_calibration_once(void *input_value, void *output_value,
                                      uint8_t enable_flag)
{
    uint8_t count_x = 0, count_y = 0;
    uint32_t adc_value_2 = 0;
    uint16_t adc_total_vcc_value[SENSOR_POS_Y] = {0};

    if(input_value == NULL) return;
    if(output_value == NULL) return;


    for (count_y = 0; count_y < SENSOR_POS_Y; count_y ++)
    {
        for (count_x = 0; count_x < SENSOR_POS_X; count_x ++)
        {
            adc_total_vcc_value[count_y] += ((uint16_t (*)[SENSOR_POS_Y])input_value)[count_x][count_y];
        }

        if (adc_total_vcc_value[count_y] >=  MAX_ADC_VALUE)
            adc_total_vcc_value[count_y] = MAX_ADC_VALUE; 
    }

    for (count_y = 0; count_y < SENSOR_POS_Y; count_y ++)
    {
        for (count_x = 0; count_x < SENSOR_POS_X; count_x ++)
        {
            adc_value_2 = ((uint16_t (*)[SENSOR_POS_Y])input_value)[count_x][count_y];
            if (adc_value_2 < 1) {
                ((uint16_t (*)[SENSOR_POS_Y])output_value)[count_x][count_y] = 0;
            } else {
                if(enable_flag)
                {
                    ((uint16_t (*)[SENSOR_POS_Y])output_value)[count_x][count_y]
                        = 255 * adc_value_2 / (MAX_ADC_VALUE - (adc_total_vcc_value[count_y] - adc_value_2));
                } else {
                    ((uint16_t (*)[SENSOR_POS_Y])output_value)[count_x][count_y] = ((uint16_t (*)[SENSOR_POS_Y])input_value)[count_x][count_y] / 4;
                }
            }
        }
    }

}

