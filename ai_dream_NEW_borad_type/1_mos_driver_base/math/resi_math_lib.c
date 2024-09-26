#include "resi_math_lib.h"

#define MATH_CALI_DEAD_VALUE 5


uint16_t g_max_resi_cal = 3000;
uint16_t g_min_resi_cal = 20;
uint16_t g_resi_ref_cal = 100;
uint16_t g_adc_max_value = 4096;
float g_min_display_value = 0.0;
float g_amplify_value = 0.0;

void math_resi_init(uint16_t max_display_resi_cal,
        uint16_t min_display_resi_cal,
        uint16_t resi_ref_value,
        uint16_t adc_max_value,
        uint16_t max_display)
{
    g_max_resi_cal = max_display_resi_cal;
    g_min_resi_cal = min_display_resi_cal;
    g_amplify_value = max_display / ((float)g_resi_ref_cal / g_min_resi_cal - (float)g_resi_ref_cal/ g_max_resi_cal);
    g_min_display_value = g_amplify_value * g_resi_ref_cal / g_max_resi_cal;
    g_resi_ref_cal = resi_ref_value;
    g_adc_max_value = adc_max_value;
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
                    resi_cali_value = (uint32_t)g_resi_ref_cal * math_resi_cal->resi_adc_value[count]
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
                if (math_resi_cal->sensor_adc_value[count] < g_adc_max_value - MATH_CALI_DEAD_VALUE)
                {
                    resi_cali_value = (uint32_t)g_resi_ref_cal * (g_adc_max_value - math_resi_cal->resi_adc_value[count])
                        / (g_adc_max_value - math_resi_cal->sensor_adc_value[count]);
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

void math_display_resi(uint16_t *math_resi_buffer,
        uint8_t *display_buffer,
        uint16_t buffer_size,
        MATH_RESI_DISPLAY_TYPE_T type)
{
    uint16_t count = 0;
    for (; count < buffer_size; count ++)
    {
        switch(type)
        {
            case RESI_SCALE:
                display_buffer[count] = math_resi_buffer[count] / 16;
                break;
            case RESI_BACKWARDS:
                display_buffer[count] = g_amplify_value * g_resi_ref_cal / math_resi_buffer[count] - g_min_display_value;
                break;
            default:
                break;
        }
    }
}
