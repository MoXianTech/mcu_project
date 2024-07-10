#ifndef _BSP_POWER_CHECK_H
#define _BSP_POWER_CHECK_H
#include "gd32f4xx.h"
#include "board.h"

#define RCU_POWER_CHECK 		RCU_GPIOE
#define PORT_POWER_CHECK		GPIOE
#define GPIO_POWER_CHECK		GPIO_PIN_13

#define GET_POWER_CHECK_IN	gpio_input_bit_get(PORT_POWER_CHECK, GPIO_POWER_CHECK)

void powercheck_thread_init(void);
void powercheck_thread_run(void);

#define ENABLE_EXTI_POWERCHECK

#endif
