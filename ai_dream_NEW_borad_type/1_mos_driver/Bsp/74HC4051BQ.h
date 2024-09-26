#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"
#include "main.h"
#include "resi_math_lib.h"

#define UART_TX(a)	if (a)	\
    gpio_bit_set(GPIOA, GPIO_PIN_9);\
    else		\
    gpio_bit_reset(GPIOA, GPIO_PIN_9)

#define XC_A(a)	if (a)	\
    gpio_bit_set(GPIOA, GPIO_PIN_10);\
    else		\
    gpio_bit_reset(GPIOA, GPIO_PIN_10)

#define XC_B(a)	if (a)	\
    gpio_bit_set(GPIOA, GPIO_PIN_9);\
    else		\
    gpio_bit_reset(GPIOA, GPIO_PIN_9)

#define XC_C(a)	if (a)	\
    gpio_bit_set(GPIOA, GPIO_PIN_8);\
    else		\
    gpio_bit_reset(GPIOA, GPIO_PIN_8)


#define SENSOR_POS_X 32
#define SENSOR_POS_Y 64

typedef enum {
    SINGLE_VCC_READ,
    TOTAL_VCC_READ,
    SINGLE_RESI_READ
} ADC_MODE_TYPE_T;

#define CACHE_BUFFER_SIZE 64

typedef struct {
    uint8_t printf_buffer[CACHE_BUFFER_SIZE];
    uint8_t recv_cache[CACHE_BUFFER_SIZE];
    uint16_t adc_sensor_value[SENSOR_POS_X][SENSOR_POS_Y];
    uint16_t adc_resi_value[SENSOR_POS_Y];
    uint16_t resi_cali_value[SENSOR_POS_X][SENSOR_POS_Y];

    uint8_t matrix_real[SENSOR_POS_X][SENSOR_POS_Y];
    uint8_t matrix_stab[SENSOR_POS_X][SENSOR_POS_Y];

    uint8_t display_matrix[SENSOR_POS_X][SENSOR_POS_Y];
    uint8_t x_max;
    uint8_t y_max;
} process_handle_t;

void adc_value_read(process_handle_t *process_handle, uint8_t count_y, ADC_MODE_TYPE_T mode_type);
void Hc4051IoInit(void);
void select_y_control_volt(process_handle_t *process_handle, uint8_t y_value, ADC_MODE_TYPE_T adc_mode, SCAN_LEVEL_T scan_mode);
void adc_calculation_calibration(process_handle_t *process_handle);
void select_x_control(uint8_t x_value);
void cal_resi_value(process_handle_t *process_handle, uint8_t y_value, SCAN_LEVEL_T scan_level);

#endif
