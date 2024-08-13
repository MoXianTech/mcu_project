#include "resi_math_lib.h"

#define MATH_CALI_DEAD_VALUE 5

uint16_t g_max_resi_cal = 3000;
uint16_t g_min_resi_cal = 20;
float g_amplify_value = 0.0;

void math_resi_init(uint16_t max_display_resi_cal,
        uint16_t min_display_resi_cal,
        uint16_t max_display)
{
    g_max_resi_cal = max_display_resi_cal;
    g_min_resi_cal = min_display_resi_cal;
    g_amplify_value = max_display / ((float)g_max_resi_cal / g_min_resi_cal);
}

void math_resi_cali_once(math_resi_cal_t *math_resi_cal,
        uint16_t *math_resi_buffer,
        uint16_t math_resi_buffer_size,
        SCAN_LEVEL_T scan_level)
{
    uint8_t count = 0;
    uint32_t resi_cali_value = 0;
    switch(scan_level)
    {
        case SCAN_HIGH_LEVEL:
            for(;count < math_resi_buffer_size; count ++)
            {
                if (math_resi_cal->sensor_adc_value[count] > MATH_CALI_DEAD_VALUE)
                {
                    resi_cali_value = (uint32_t)math_resi_cal->resi_value * math_resi_cal->resi_adc_value[count]
                        / math_resi_cal->sensor_adc_value[count];
                } else {
                    resi_cali_value = g_max_resi_cal;
                }

                if (resi_cali_value > g_max_resi_cal)
                    resi_cali_value = g_max_resi_cal;

                if (resi_cali_value < g_min_resi_cal)
                    resi_cali_value = g_min_resi_cal;

                math_resi_buffer[count] = resi_cali_value;
            }

            break;
        case SCAN_LOW_LEVEL:
            for(;count < math_resi_buffer_size; count ++)
            {
                if (math_resi_cal->sensor_adc_value[count] < math_resi_cal->adc_max_value - MATH_CALI_DEAD_VALUE)
                {
                    resi_cali_value = (uint32_t)math_resi_cal->resi_value * (math_resi_cal->adc_max_value - math_resi_cal->resi_adc_value[count])
                        / (math_resi_cal->adc_max_value - math_resi_cal->sensor_adc_value[count]);
                } else {
                    resi_cali_value = g_max_resi_cal;
                }

                if (resi_cali_value > g_max_resi_cal)
                    resi_cali_value = g_max_resi_cal;

                if (resi_cali_value < g_min_resi_cal)
                    resi_cali_value = g_min_resi_cal;

                math_resi_buffer[count] = resi_cali_value;
            }

            break;
        default:
            break;
    }
}

void math_display_resi(uint16_t *math_resi_buffer, uint8_t *display_buffer, uint16_t buffer_size)
{
    uint16_t count = 0;
    for (; count < buffer_size; count ++)
    {
        if (0)
        {
            display_buffer[count] = math_resi_buffer[count] / 16;
        } else {
            display_buffer[count] = g_max_resi_cal / math_resi_buffer[count];
            display_buffer[count] = g_amplify_value * display_buffer[count];
        }
    }
}
