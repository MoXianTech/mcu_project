/*************************************************************************
  > File Name: 74hc595.c
  > Author: ma6174
  > Mail: ma6174@163.com
  > Created Time: 2023/9/20 17:48:24
 ************************************************************************/

#include  "74hc595.h"
#include "gd32f4xx.h"
#include  <stdlib.h>
#include  <stdio.h>
#include  <stdint.h>
#include "systick.h"


#define DS(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_5);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_5)

#define SHCP(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_7);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_7)

#define STCP(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_6);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_6)


#define IO_MR(a) if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_8);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_8)


#define IO_DEF_SET(a)	if (a)	\
    gpio_bit_set(GPIOB, GPIO_PIN_4);\
    else		\
    gpio_bit_reset(GPIOB, GPIO_PIN_4)

inline void ic_74hc595_delay(uint32_t num)
{
    while(num --);
}

#define IO_MAX_NUM 72
int8_t io_value = 0;
SCAN_LEVEL_T base_scan_level = SCAN_HIGH_LEVEL;

SCAN_LEVEL_T get_scan_mode(void)
{
    return base_scan_level;
}

void ic_74hc595_move_clean(bool output_enable)
{
    switch(base_scan_level)
    {
        case SCAN_HIGH_LEVEL:
            DS(1);
            break;
        case SCAN_LOW_LEVEL:
            DS(0);
            break;
        default:
            break;
    }

    for (; io_value < IO_MAX_NUM; io_value ++)
    {
        SHCP(1);
        SHCP(0);
    }

    io_value = -1;

    if (output_enable)
    {
        STCP(1);
        STCP(0);
    }
}


void ic_74hc595_init(SCAN_LEVEL_T scan_level)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    //rcu_periph_clock_enable(RCU_AF);
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT,  GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_4 | GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_4 | GPIO_PIN_8);

    base_scan_level = scan_level;
    IO_MR(1);
    ic_74hc595_move_clean(true);

    IO_DEF_SET(1);
}

void ic_74hc595_full_clean(void)
{
    IO_MR(0);
    STCP(1);
    STCP(0);
    io_value = -1;
    IO_MR(1);
}

void ic_74hc595_move_bit(int8_t bit_num)
{
    uint8_t move_io_value = 0;

    if (bit_num == io_value) return;
    if (bit_num >= IO_MAX_NUM) return;
    if (bit_num < 0)
    {
        ic_74hc595_full_clean();
        return;
    }

    switch(base_scan_level)
    {
        case SCAN_HIGH_LEVEL:
            move_io_value = 1;
            break;
        case SCAN_LOW_LEVEL:
            move_io_value = 0;
            break;
        default:
            break;
    }

    if (bit_num < io_value)
    {
        ic_74hc595_move_clean(false);
    }

    if (io_value < 0)
    {
        DS(!move_io_value);
        SHCP(1);
        SHCP(0);
        io_value ++;
    }

    DS(move_io_value);

    for(; io_value < bit_num; io_value ++)
    {
        SHCP(1);
        SHCP(0);
    }

    STCP(1);
    STCP(0);

    if (bit_num >= IO_MAX_NUM) bit_num = 0;
}

void ic_74hc595_thread(void)
{
    if (io_value)
        DS(1);
    else
        DS(0);
    io_value ++;

    if (io_value == 72) io_value = 0;

    STCP(0);
    SHCP(0);
    SHCP(1);
    SHCP(0);
    STCP(1);
    STCP(0);
}

void ic_74hc595_set_io_scan(int8_t io_num)
{
    uint8_t io_value = 0x00;
    switch (base_scan_level)
    {
        case SCAN_HIGH_LEVEL:
            io_value = 0;
            break;
        case SCAN_LOW_LEVEL:
            io_value = 1;
            break;
        default:
            break;
    }

    ic_74hc595_move_bit(io_num);
    if (io_num < 0)
        IO_DEF_SET(io_value);
    else
        IO_DEF_SET(!io_value);
}
