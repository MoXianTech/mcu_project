#include <stdlib.h>
#include <stdio.h>
#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include "oled.h"


#define ADC_DELAY_TIME (12 * 120) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}


void Hc4051IoInit(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);

    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    gpio_bit_set(GPIOB, GPIO_PIN_2);
    XC_A(0);
    XC_B(0);
    XC_C(1);

    //math_resi_init(6000, 100, 100, 4096, 255); //tpv8021B damo
    //math_resi_init(6000, 350, 100, 4096, 255); //tpus8022A test auto
	//math_resi_init(6000, 350, 100, 4096, 255); //tpus919 test auto
    //math_resi_init(6000, 350, 100, 4096, 255); //tpus8022A test auto
	//math_resi_init(6000, 200, 100, 4096, 255); //tpus8022A øπ»‰±‰
		math_resi_init(6000, 400, 100, 4096, 255); //tpus 919 øπ»‰±‰
}

uint8_t adc_rank[8] = {0, 3, 2, 1, 5, 6 ,7, 4};
uint8_t type_a_rank[8] = {6, 5, 4, 7, 0, 3, 1, 2};
uint8_t type_b_rank[8] = {5, 6, 7, 4, 3, 0, 2, 1};

uint8_t adc_channel_choice_num[8] = {6, 5, 4, 7, 3, 0, 2, 1};



void adc_value_read(process_handle_t *process_handle, uint8_t count_y, ADC_MODE_TYPE_T mode_type)
{
    uint8_t x_array = 0;
    uint8_t count = 0, count_1 = 0;

    switch (mode_type)
    {
        case SINGLE_VCC_READ:
            for (count = 0; count < 8; count ++)
            {
                select_x_control(count);
                Hc4051Delay(ADC_DELAY_TIME);
                for (count_1 = 0; count_1 < 8; count_1 ++)
                {
                    x_array = count_1 * 8;
                    x_array += adc_channel_choice_num[count];

                    process_handle->adc_sensor_value[count_y][x_array] = GetAdcValue(adc_rank[count_1]);
                }
            }
            break;

        case SINGLE_RESI_READ:
            for (count = 0; count < 8; count ++)
            {
                select_x_control(count);
                Hc4051Delay(ADC_DELAY_TIME);
                for (count_1 = 0; count_1 < 8; count_1 ++)
                {
                    x_array = count_1 * 8;
                    x_array += adc_channel_choice_num[count];

                    process_handle->adc_resi_value[x_array] = GetAdcValue(adc_rank[count_1]);
                }
            }
            break;


        default:
            break;
    }

}

void select_x_control(uint8_t x_value)
{
    uint16_t x_default = 0x00;
    uint16_t hardware_pint = 0;

    x_default = gpio_output_port_get(GPIOA);

    x_default &= 0xF8FF;

    hardware_pint |= (x_value & 0x01) << 2;
    hardware_pint |= (x_value & 0x02);
    hardware_pint |= (x_value & 0x04) >> 2;

    x_default |= (hardware_pint << 8);

    gpio_port_write(GPIOA, x_default);

}

uint8_t refine_y_pos_normal[32] = {22, 21, 20, 19, 18, 17, 16, 7,
    6,  5,  4,  3, 2, 1,  0,  15,
    //    8, 9,  10,  11,  12,  13, 14,  31,
    14, 13, 12, 11, 10, 9 , 8, 31,
    30, 29, 28, 27, 26, 25, 24, 23};

uint8_t refine_y_pos_aosha[32] = {19, 18, 17, 16, 31, 30, 29, 20,
    21,  0,  1,  2, 10, 11,  9,  8,
    12, 13, 14, 15, 22, 23, 24, 25,
    26, 27, 28,  3,  4,  5,  6,  7};

void select_y_control_volt(process_handle_t *process_handle,
        uint8_t y_value,
        ADC_MODE_TYPE_T adc_mode,
        SCAN_LEVEL_T scan_mode)
{
    uint32_t y_target_value = 0x01;
    bool enable_negation = false;

    if (scan_mode == SCAN_HIGH_LEVEL) enable_negation = true;
    if (scan_mode == SCAN_LOW_LEVEL) enable_negation = false;

    switch (adc_mode)
    {
        case SINGLE_VCC_READ:
            y_value = refine_y_pos_normal[y_value];
            if (enable_negation)
                gpio_bit_set(GPIOC, GPIO_PIN_0);
            else
                gpio_bit_reset(GPIOC, GPIO_PIN_0);
            if (y_value > 15) {
                y_value -= 16;
                y_target_value = y_target_value << y_value;
                if (enable_negation)
                {
                    y_target_value = ~y_target_value;
                    gpio_port_write(GPIOD, 0xffff);
                } else {
                    gpio_port_write(GPIOD, 0x0000);
                }

                gpio_port_write(GPIOE, y_target_value);
            } else {
                y_target_value = y_target_value << y_value;
                if (enable_negation)
                {
                    y_target_value = ~y_target_value;
                    gpio_port_write(GPIOE, 0xffff);
                } else {
                    gpio_port_write(GPIOE, 0x0000);
                }

                gpio_port_write(GPIOD, y_target_value);
            }
            break;

        case SINGLE_RESI_READ:
            if (enable_negation)
            {
                gpio_port_write(GPIOE, 0xffff);
                gpio_port_write(GPIOD, 0xffff);
                gpio_bit_reset(GPIOC, GPIO_PIN_0);
            } else {
                gpio_port_write(GPIOE, 0x0000);
                gpio_port_write(GPIOD, 0x0000);
                gpio_bit_set(GPIOC, GPIO_PIN_0);
            }
            break;


        default:

            break;
    }
}

void cal_resi_value(process_handle_t *process_handle, uint8_t y_value, SCAN_LEVEL_T scan_level)
{
    math_resi_cal_t math_resi_cal = {0};
    math_resi_cal.sensor_adc_value = process_handle->adc_sensor_value[y_value];
    math_resi_cal.resi_adc_value = process_handle->adc_resi_value;

    math_resi_cali_once(&math_resi_cal,
            process_handle->resi_cali_value[y_value],
            process_handle->y_max,
            scan_level);

    math_display_resi(process_handle->resi_cali_value[y_value],
            process_handle->matrix_real[y_value],
            process_handle->y_max,
            RESI_BACKWARDS);

    if (0)
    {
        if (y_value == 0)
            printf("%d/%dresi %d %d\n",
                    process_handle->adc_sensor_value[0][1],
                    process_handle->adc_resi_value[1],
                    process_handle->resi_cali_value[0][1],
                    process_handle->matrix_display[0][1]);
    }
}

#define CREEP_VALUR 1
#define CREEP_INTERVAL_FRAME 5
void cal_creep_resistance(process_handle_t *process_handle)
{
    uint16_t count = SENSOR_POS_X * SENSOR_POS_Y;
    uint8_t *matrix_real = (uint8_t *)process_handle->matrix_real;
    uint8_t *matrix_stab = (uint8_t *)process_handle->matrix_stab;
    uint8_t *matrix_creep = (uint8_t *)process_handle->matrix_creep;
    uint8_t *matrix_display = (uint8_t *)process_handle->matrix_display;
    int16_t matrix_deviation = 0;
    static uint64_t frame_count = 0;

    frame_count ++;

    do {
        count --;

        if (matrix_real[count] < 2)
        {
            matrix_display[count] = 0;
            matrix_creep[count] = 0;
            continue;
        }

        if (!(frame_count % CREEP_INTERVAL_FRAME))
        {
            matrix_deviation = (matrix_real[count] > matrix_stab[count]) ? (matrix_real[count] - matrix_stab[count]) : (matrix_stab[count] - matrix_real[count]);

            if (matrix_deviation > CREEP_VALUR)
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

#if 0

#define CREEP_VALUR 1
void cal_creep_resistance(process_handle_t *process_handle)
{
    uint16_t count = process_handle->x_max * process_handle->y_max;
    uint8_t *matrix_real = (uint8_t *)process_handle->matrix_real;
    uint8_t *matrix_stab = (uint8_t *)process_handle->matrix_stab;
    uint8_t *matrix_creep = (uint8_t *)process_handle->matrix_creep;
    uint8_t *matrix_display = (uint8_t *)process_handle->matrix_display;
    int16_t matrix_deviation = 0;

    do {
        count --;
        matrix_deviation = (matrix_real[count] > matrix_stab[count]) ? (matrix_real[count] - matrix_stab[count]) : (matrix_stab[count] - matrix_real[count]);
        //matrix_deviation = (matrix_real[count] > matrix_stab[count]) ? (matrix_real[count] - matrix_stab[count]) : -1;

        if (matrix_deviation > CREEP_VALUR)
        {
            matrix_display[count] = matrix_real[count] > matrix_creep[count] ? matrix_real[count] - matrix_creep[count] : matrix_real[count];
        } else {
            matrix_deviation = (matrix_real[count] > matrix_stab[count]) ? matrix_deviation : -matrix_deviation;
            matrix_creep[count] += matrix_deviation;
        }

        if (matrix_real[count] < 10)
        {
            matrix_creep[count] = 0;
            matrix_display[count] = 0;
        }
        matrix_stab[count] = matrix_real[count];

    } while(count);
}
#endif

