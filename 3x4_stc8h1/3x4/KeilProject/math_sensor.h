/*************************************************************************
	> File Name: math_sensor.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/10/26 16:34:10
 ************************************************************************/

#include "type_def.h"

#define SENSOR_POS_X 3
#define SENSOR_POS_Y 4

#define MAX_ADC_VALUE 1024

void adc_calculation_calibration_once(void *input_value, void *output_value,
                                      uint8_t enable_flag);
