#ifndef __BSP_ENCODER_H
#define __BSP_ENCODER_H
#include "gd32f4xx.h"
#include "board.h"

#define ENCODER_TIMER_RCU    RCU_TIMER5  		// 定时器时钟
#define ENCODER_TIMER   	  	TIMER5   			// 定时器
#define ENCODER_TIMER_IRQ  	TIMER5_DAC_IRQn   	// 定时器中断

#define RCU_ENCODER_A		RCU_GPIOE
#define RCU_ENCODER_B		RCU_GPIOE
#define RCU_ENCODER_C		RCU_GPIOE
#define RCU_ENCODER_D		RCU_GPIOE

#define PORT_ENCODER_A	GPIOE
#define PORT_ENCODER_B	GPIOE
#define PORT_ENCODER_C	GPIOE
#define PORT_ENCODER_D  GPIOE

#define GPIO_ENCODER_A	GPIO_PIN_11
#define GPIO_ENCODER_B	GPIO_PIN_10
#define GPIO_ENCODER_C	GPIO_PIN_9
#define GPIO_ENCODER_D  GPIO_PIN_8

#define GET_ENCODER_A_IN	gpio_input_bit_get(PORT_ENCODER_A, GPIO_ENCODER_A)
#define GET_ENCODER_B_IN	gpio_input_bit_get(PORT_ENCODER_B, GPIO_ENCODER_B)
#define GET_ENCODER_C_IN	gpio_input_bit_get(PORT_ENCODER_C, GPIO_ENCODER_C)
#define GET_ENCODER_D_IN	gpio_input_bit_get(PORT_ENCODER_D, GPIO_ENCODER_D)

#define ENCODER_A_EXIT_IRQn		EXTI10_15_IRQn	
#define ENCODER_B_EXIT_IRQn		EXTI10_15_IRQn
#define ENCODER_C_EXIT_IRQn		EXTI5_9_IRQn
#define ENCODER_D_EXIT_IRQn		EXTI5_9_IRQn

#define ENCODER_A_EXTI_PORT_SOURCE		EXTI_SOURCE_GPIOE
#define ENCODER_B_EXTI_PORT_SOURCE		EXTI_SOURCE_GPIOE
#define ENCODER_C_EXTI_PORT_SOURCE		EXTI_SOURCE_GPIOE
#define ENCODER_D_EXTI_PORT_SOURCE		EXTI_SOURCE_GPIOE

#define ENCODER_A_EXTI_PIN_SOURCE			EXTI_SOURCE_PIN11
#define ENCODER_B_EXTI_PIN_SOURCE			EXTI_SOURCE_PIN10
#define ENCODER_C_EXTI_PIN_SOURCE			EXTI_SOURCE_PIN9
#define ENCODER_D_EXTI_PIN_SOURCE			EXTI_SOURCE_PIN8

#define ENCODER_A_EXTI_LINE		EXTI_11
#define ENCODER_B_EXTI_LINE		EXTI_10
#define ENCODER_C_EXTI_LINE		EXTI_9
#define ENCODER_D_EXTI_LINE		EXTI_8


typedef enum
{
	MOTO_A = 0,
	MOTO_B,
	MOTO_C,
	MOTO_D,
	MOTO_MAX_NUM
}moto_num;


void encoder_init(void);
uint32_t get_encoder_currentCount(moto_num encoder);
void set_encoder_currentCount(moto_num encoder, int16_t current_count);
void clear_encoder_currentCount(moto_num encoder);
uint32_t get_encoder_speed(moto_num encoder);
void lock_encoder_currentCount(moto_num encoder);
void unlock_encoder_currentCount(moto_num encoder);

#endif
