#ifndef __PROCESS_R_VALUE
#define __PROCESS_R_VALUE

#include "gd32e10x.h"


/************************Function Input************************/
/*r_ref_adc_value     ----   Reference Resisitance adc Value*/
/*r_sensor_adc_value  ----   Sensor Array adc Value*/
/*cal_r_value_output  ----   Calculated Resistance Value*/
/*adc_y_num           ----   Total adc(Column) Number*/
/*Rref_value          ----   Reference Resistance(Omu)*/
/*Vref_value          ----   Power voltage(eg. 4096)*/
void process_adc_to_resistance( uint16_t r_ref_adc_value[], 
																uint16_t r_sensor_adc_value[],
																uint32_t cal_r_value_output[],	
																uint16_t total_adc_y_num,
																uint16_t Rref_value,
																uint16_t Vref_value
																);

#endif


