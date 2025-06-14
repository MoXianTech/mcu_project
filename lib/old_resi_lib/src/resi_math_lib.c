#include "resi_math_lib.h"


#define MATH_CALI_DEAD_VALUE 5

uint8_t g_resi_ref_cal = 0;
uint16_t g_ref_display_max = 100;
uint32_t g_adc_max_value = 4096;
uint32_t g_min_display_value = 0;
uint32_t g_max_display_value = 0;
uint32_t g_max_display_value_out = 0;

float g_amplify_value = 0.0;

void math_resi_init(uint16_t min_display_value,
        uint16_t max_display_value,
        uint16_t max_value,
        uint16_t adc_max_value)
{
    g_ref_display_max = max_value;
    g_adc_max_value = adc_max_value;
    g_max_display_value = g_adc_max_value * max_display_value / max_value;
    g_min_display_value = g_adc_max_value * min_display_value / max_value;
    g_amplify_value = adc_max_value / max_value;
}


/*
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
   */

void math_resi_cali_single_sweep(void *sensor_adc_value,
        void *math_resi_value,
        DATA_BIT_SIZE_T data_bit_size,
        uint16_t value_count,
        SCAN_LEVEL_T scan_level)
{
    uint8_t *sensor_adc_value_single_pointer = 0, *math_resi_value_single_pointer = 0;
    uint16_t *sensor_adc_value_double_pointer = 0, *math_resi_value_double_pointer = 0;
    uint32_t total_adc_value = 0, math_cali_adc_value_cache = 0;
    uint16_t sensor_value_count = 0;

    if (scan_level == SCAN_HIGH_LEVEL_SINGLE_SWEEP)
    {
        switch (data_bit_size)
        {
            case DOUBLE_BYTE:
                sensor_adc_value_double_pointer = (uint16_t *)sensor_adc_value;
                math_resi_value_double_pointer = (uint16_t *)math_resi_value;
                for (; sensor_value_count < value_count; sensor_value_count ++)
                {
                    total_adc_value += sensor_adc_value_double_pointer[sensor_value_count];
                }

                math_cali_adc_value_cache = g_adc_max_value - total_adc_value;

                for (sensor_value_count = 0; sensor_value_count < value_count; sensor_value_count ++)
                {
                    if (0)
                    math_resi_value_double_pointer[sensor_value_count] =
                        (uint32_t)math_cali_adc_value_cache * g_resi_ref_cal
                        / sensor_adc_value_double_pointer[sensor_value_count];
                    else {
                        math_resi_value_double_pointer[sensor_value_count] =
                            (uint32_t)(sensor_adc_value_double_pointer[sensor_value_count]) * g_adc_max_value /
                            (g_adc_max_value - (total_adc_value - sensor_adc_value_double_pointer[sensor_value_count]));
                    }
                }

                break;
            case SINGLE_BYTE:
                sensor_adc_value_single_pointer = (uint8_t *)sensor_adc_value;
                math_resi_value_single_pointer = (uint8_t *)math_resi_value;
                for (; sensor_value_count < value_count; sensor_value_count ++)
                {
                    total_adc_value += sensor_adc_value_single_pointer[sensor_value_count];
                }

                math_cali_adc_value_cache = g_adc_max_value - total_adc_value;

                for (sensor_value_count = 0; sensor_value_count < value_count; sensor_value_count ++)
                {
                    math_resi_value_single_pointer[value_count] =
                        (uint32_t)math_cali_adc_value_cache * g_resi_ref_cal
                        / sensor_adc_value_single_pointer[sensor_value_count];
                }

                break;
        }



    }
}


void math_resi_cali(math_resi_cal_t *math_resi_cal)
{
#if 1
    switch (math_resi_cal->scan_level)
    {
        case SCAN_HIGH_LEVEL_DOUBLE_SWEEP:
        case SCAN_LOW_LEVEL_DOUBLE_SWEEP:

            break;

        case SCAN_HIGH_LEVEL_SINGLE_SWEEP:
        case SCAN_LOW_LEVEL_SINGLE_SWEEP:
            math_resi_cali_single_sweep(math_resi_cal->sensor_adc_value,
                    math_resi_cal->math_resi_value,
                    math_resi_cal->data_bit_size,
                    math_resi_cal->value_count,
                    math_resi_cal->scan_level);
            break;
        default:
            break;
    }
#endif
}

void math_display_resi(uint16_t *math_resi_buffer,
        void *display_buffer,
        uint16_t buffer_size,
        DATA_BIT_SIZE_T data_bit_size,
        MATH_RESI_DISPLAY_TYPE_T type)
{
    uint16_t count = 0;
    uint8_t *single_point = 0;
    uint16_t *double_point = 0;

    switch (data_bit_size)
    {
        case SINGLE_BYTE:
            single_point = (uint8_t *)display_buffer;
            for (; count < buffer_size; count ++)
            {
                switch(type)
                {
                    case RESI_SCALE:
#if 1
                        if (math_resi_buffer[count] < g_min_display_value)
                            math_resi_buffer[count] = g_min_display_value;
                        if (math_resi_buffer[count] > g_max_display_value)
                            math_resi_buffer[count] = g_max_display_value;
#endif
                        single_point[count] = g_ref_display_max *
                            (math_resi_buffer[count] - g_min_display_value)
                            / (g_max_display_value - g_min_display_value);
                        //single_point[count] = math_resi_buffer[count] / 16;
                        break;
                    case RESI_BACKWARDS:
                        single_point[count] = g_amplify_value * g_resi_ref_cal
                            / math_resi_buffer[count] - g_min_display_value;
                        break;
                    default:
                        break;
                }
            }
            break;
        case DOUBLE_BYTE:
            double_point = (uint16_t *)display_buffer;
            for (; count < buffer_size; count ++)
            {
                switch(type)
                {
                    case RESI_SCALE:
                        double_point[count] = math_resi_buffer[count] / 16;
                        break;
                    case RESI_BACKWARDS:
                        double_point[count] = g_amplify_value * g_resi_ref_cal
                            / math_resi_buffer[count] - g_min_display_value;
                        break;
                    default:
                        break;
                }
            }
            break;
        default:

            break;
    }
}
