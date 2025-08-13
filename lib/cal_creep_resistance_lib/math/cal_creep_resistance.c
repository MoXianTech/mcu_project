#include "cal_creep_resistance.h"

#define CREEP_VALUE 1
#define CREEP_INTERVAL_FRAME 5

uint8_t g_creep_value = CREEP_VALUE;
uint8_t g_creep_interval_frame = CREEP_INTERVAL_FRAME;

void cal_creep_resistance_init(uint8_t creep_strength, uint8_t creep_level)
{
	g_creep_value = creep_strength;
	g_creep_interval_frame = creep_level;
}

void cal_creep_resistance(cal_creep_resistance_t *cal_creep_resistance)
{
	  static uint64_t frame_count = 0;
    uint16_t count = cal_creep_resistance->x_max * cal_creep_resistance->y_max;
    uint8_t *matrix_real = (uint8_t *)cal_creep_resistance->matrix_real;
    uint8_t *matrix_stab = (uint8_t *)cal_creep_resistance->matrix_stab;
    uint8_t *matrix_creep = (uint8_t *)cal_creep_resistance->matrix_creep;
    uint8_t *matrix_display = (uint8_t *)cal_creep_resistance->matrix_display;
    int16_t matrix_deviation = 0;

    frame_count ++;

    do {
        count --;

        if (matrix_real[count] < 2)
        {
            matrix_display[count] = 0;
            matrix_creep[count] = 0;
            continue;
        }

        if (!(frame_count % g_creep_interval_frame))
        {
            matrix_deviation = (matrix_real[count] > matrix_stab[count]) ? (matrix_real[count] - matrix_stab[count]) : (matrix_stab[count] - matrix_real[count]);

            if (matrix_deviation > g_creep_value)
            {

            } else {
                matrix_deviation = (matrix_real[count] > matrix_stab[count]) ? matrix_deviation : -matrix_deviation;
                matrix_creep[count] += matrix_deviation;
            }

            matrix_stab[count] = matrix_real[count];
        }

        matrix_display[count] = matrix_real[count] > matrix_creep[count] ? matrix_real[count] - matrix_creep[count] : matrix_real[count];

        if (matrix_real[count] < matrix_creep[count])
        {
            matrix_creep[count] = 0;
        }


    } while(count);
}
