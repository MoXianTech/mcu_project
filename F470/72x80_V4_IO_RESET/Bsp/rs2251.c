#include <stdlib.h>
#include <stdio.h>
#include "rs2251.h"
#include "adc.h"
#include "math.h"
#include "string.h"

#define RS2251_A(a)  if (a) gpio_bit_set(GPIOE, GPIO_PIN_1); else gpio_bit_reset(GPIOE, GPIO_PIN_1);
#define RS2251_B(a)  if (a) gpio_bit_set(GPIOE, GPIO_PIN_0); else gpio_bit_reset(GPIOE, GPIO_PIN_0);
#define RS2251_C(a)  if (a) gpio_bit_set(GPIOB, GPIO_PIN_9); else gpio_bit_reset(GPIOB, GPIO_PIN_9);

#define ADC_DELAY_TIME (600) //9 ns * 120

inline void rs2251Delay(uint32_t num)
{
    while(num --);
}

void rs2251_io_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOB);
    //rcu_periph_clock_enable(RCU_AF);
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);

    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    math_resi_init(6000, 200, 100, 4063, 255);
}

uint8_t adc_rank_value[10] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9};
//uint8_t HC595_rank_value[8] = {7, 0, 1, 2, 3, 6, 5, 4};
uint8_t HC595_rank_value[8] = {0, 7, 6, 5, 4, 1, 2, 3};

void adc_value_read(process_handle_t *process_handle, uint8_t count_y, ADC_MODE_TYPE_T mode_type)
{
    uint8_t count = 0, count_rs2251 = 0;
    uint16_t adc_value = 0;
    uint8_t count_y_remap = 0;
    uint16_t *write_buffer = NULL;

    count_y_remap = (count_y / 8) * 8 + HC595_rank_value[(count_y % 8)];

    switch(mode_type)
    {
        case SINGLE_VCC_READ:
            write_buffer = process_handle->adc_sensor_value[count_y_remap];
            break;
        case SINGLE_RESI_READ:
            write_buffer = process_handle->adc_resi_value;
            break;
        default:
            break;
    }

		if (1)
    for (count_rs2251 = 0; count_rs2251 < 8; count_rs2251 ++)
    {
        select_x_control(count_rs2251);
        rs2251Delay(ADC_DELAY_TIME);
        for (count = 0; count < 10; count ++)
        {
            adc_value  = GetAdcValue(count);
            write_buffer[adc_rank_value[count] * 8 + count_rs2251] = adc_value;
        }
    }
		else {
		    for (count_rs2251 = 0; count_rs2251 < 8; count_rs2251 ++)
				{
						select_x_control(count_rs2251);
						rs2251Delay(ADC_DELAY_TIME);
						for (count = 0; count < 10; count ++)
						{
								adc_value  = GetAdcValue(count);
								write_buffer[adc_rank_value[count] * 8 + count_rs2251] = adc_value;
						}
				}
		}
}

uint8_t single_adc_rank[8] = {2, 1, 0, 3, 4, 6, 7, 5};
void select_x_control(uint8_t x_value)
{
    uint8_t rank_value = single_adc_rank[x_value];

    RS2251_A(rank_value & 0x01);
    RS2251_B(rank_value >> 1 & 0x01);
    RS2251_C(rank_value >> 2 & 0x01);
}

void cal_resi_value(process_handle_t *process_handle, uint8_t y_value)
{
    math_resi_cal_t math_resi_cal = {0};
    math_resi_cal.sensor_adc_value = process_handle->adc_sensor_value[y_value];
    math_resi_cal.resi_adc_value = process_handle->adc_resi_value;

    math_resi_cali_once(&math_resi_cal,
            process_handle->resi_cali_value[y_value],
            process_handle->y_max,
            get_scan_mode());

    math_display_resi(process_handle->resi_cali_value[y_value],
            process_handle->display_matrix[y_value],
            process_handle->y_max,
            RESI_BACKWARDS,
            4);

    if (0)
    {
        if (y_value == 0)
            sprintf((char *)process_handle->printf_buffer, "sensor/resi %d/%d resi %d %d\n",
                    process_handle->adc_sensor_value[0][0],
                    process_handle->adc_resi_value[0],
                    process_handle->resi_cali_value[0][0],
                    process_handle->display_matrix[0][0]);
    }

}


