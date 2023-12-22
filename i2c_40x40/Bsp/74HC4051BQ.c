#include <stdlib.h>
#include <stdio.h>
#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include "oled.h"


#define ADC_DELAY_TIME (15*120) //9 ns * 120

inline void rs2251Delay(uint32_t num)
{
    while(num --);
}

void rs2251IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
}

uint8_t adc_rank[16] = { 9,  8, 15, 14,
                         7,  6,  5,  4,
                         3,  2,  1,  0,
                        13, 12, 11, 10};

uint8_t adc_rank_value[5] = {0, 3, 4, 1, 2};


void adc_value_read(process_handle_t *process_handle, uint8_t count_y)
{
    uint8_t count = 0, count_rs2251 = 0;
    uint16_t adc_value = 0;
#if 0
    sprintf((char *)process_handle->printf_buffer, "%d %d %d %d %d \n", GetAdcValue(0),
                                                   GetAdcValue(1),
                                                   GetAdcValue(2),
                                                   GetAdcValue(3),
                                                   GetAdcValue(4)
                                                   );

#else
    for (count_rs2251 = 0; count_rs2251 < 8; count_rs2251 ++)
    {
        select_x_control(count_rs2251);
        rs2251Delay(ADC_DELAY_TIME);
        for (count = 0; count < 5; count ++)
        {
            adc_value  = GetAdcValue(count);
            if (adc_value > 4090)
                adc_value = 4090;

            process_handle->adc_raw_value[count_y][adc_rank_value[count] * 8 + count_rs2251]
                = adc_value;
        }
    }
#endif
}

//uint8_t single_adc_rank[8] = {2, 1, 0, 3, 5, 7, 6, 4};
uint8_t single_adc_rank[8] = {4, 6, 7, 5, 3, 0, 1, 2};
void select_x_control(uint8_t x_value)
{
    uint16_t y_default = 0x00;

    y_default = gpio_output_port_get(GPIOC);

    y_default &= 0xfc7f;

    y_default |= (single_adc_rank[x_value] << 7);

    gpio_port_write(GPIOC, y_default);
}

void select_y_control(process_handle_t *process_handle, uint8_t y_value)
{
    uint16_t y_default = 0x00;
    uint8_t hardware_pint = 0;
    bool control_down_flag = true;

    y_default = gpio_output_port_get(GPIOB);
    y_default &= 0x0ff8;
    switch(y_value)
    {
        case 0: case 1: case 2: case 3:
        case 4: case 5: case 6: case 7:
            control_down_flag = true;
            y_default |= 0x7000;
            break;
        case 8: case 9: case 10: case 11:
        case 12: case 13: case 14: case 15:
            control_down_flag = false;
            y_default |= 0xB000;
            break;
        case 16: case 17: case 18: case 19:
        case 20: case 21: case 22: case 23:
            control_down_flag = false;
            y_default |= 0xD000;
            break;
        case 24: case 25: case 26: case 27:
        case 28: case 29: case 30: case 31:
            control_down_flag = true;
            y_default |= 0xE000;
            break;
        default:
            break;
    }

    if (control_down_flag)
    {
        switch (y_value % 8)
        {
            case 0:
                y_value = 4;
                break;
            case 1:
                y_value = 6;
                break;
            case 2:
                y_value = 7;
                break;
            case 3:
                y_value = 5;
                break;
            case 4:
                y_value = 2;
                break;
            case 5:
                y_value = 1;
                break;
            case 6:
                y_value = 0;
                break;
            case 7:
                y_value = 3;
                break;
            default:
                break;
        }
    }
    else {
        switch (y_value % 8)
        {
            case 0:
                y_value = 5;
                break;
            case 1:
                y_value = 7;
                break;
            case 2:
                y_value = 6;
                break;
            case 3:
                y_value = 4;
                break;
            case 4:
                y_value = 3;
                break;
            case 5:
                y_value = 0;
                break;
            case 6:
                y_value = 1;
                break;
            case 7:
                y_value = 2;
                break;
            default:
                break;
        }
    }

    hardware_pint |= (y_value & 0x01) << 2;
    hardware_pint |= (y_value & 0x02);
    hardware_pint |= (y_value & 0x04) >> 2;

    y_default |= (hardware_pint & 0x7);
    gpio_port_write(GPIOB, y_default);
}

void adc_calculation_calibration_once(process_handle_t *process_handle, bool enable_flag)
{
    uint8_t count_x = 0, count_y = 0;
    uint32_t adc_value_2 = 0;
    for (count_y = 0; count_y < SENSOR_POS_Y; count_y ++)
    {
        for (count_x = 0; count_x < SENSOR_POS_X; count_x ++)
        {
						if (process_handle->adc_raw_value[count_x][count_y] < 10) continue;
            process_handle->adc_total_vcc_value[count_y] += process_handle->adc_raw_value[count_x][count_y];
        }

        if (process_handle->adc_total_vcc_value[count_y] >=  4096)
            process_handle->adc_total_vcc_value[count_y] = 4096; 
    }

    for (count_y = 0; count_y < SENSOR_POS_Y; count_y ++)
    {
        for (count_x = 0; count_x < SENSOR_POS_X; count_x ++)
        {
            adc_value_2 = process_handle->adc_raw_value[count_x][count_y];
            if (adc_value_2 < 3) {
                process_handle->adc_cali_value[count_x][count_y] = 0;
								process_handle->adc_raw_value[count_x][count_y] = 0;
            } else {
                if(enable_flag)
                {
                    adc_value_2 = 512 * adc_value_2 / (4030 - (process_handle->adc_total_vcc_value[count_y] - adc_value_2));

										process_handle->adc_cali_value[count_x][count_y] = adc_value_2 > 255 ? 255: adc_value_2;
                } else {
                    process_handle->adc_cali_value[count_x][count_y] = adc_value_2 / 16;
                }
            }

        }
    }

    memset(process_handle->adc_total_vcc_value, 0x00, 4 * SENSOR_POS_Y);
}


