#ifndef _BSP_L298N_H
#define _BSP_L298N_H

#include "gd32f4xx.h"
#include "board.h"

#define RCU_IN1                 RCU_GPIOB
#define PORT_IN1                GPIOB
#define GPIO_IN1                GPIO_PIN_14
#define AF_IN1                  GPIO_AF_9

#define RCU_IN2                 RCU_GPIOB
#define PORT_IN2                GPIOB
#define GPIO_IN2                GPIO_PIN_15
#define AF_IN2                  GPIO_AF_9

#define RCU_IN3                 RCU_GPIOB
#define PORT_IN3                GPIOB
#define GPIO_IN3                GPIO_PIN_1
#define AF_IN3                  GPIO_AF_2

#define RCU_IN4                 RCU_GPIOB
#define PORT_IN4                GPIOB
#define GPIO_IN4                GPIO_PIN_0
#define AF_IN4                  GPIO_AF_2

#define RCU_IN1_TIMER        	RCU_TIMER11
#define BSP_IN1_TIMER        	TIMER11        // 定时器
#define BSP_IN1_CHANNEL        	TIMER_CH_0     // 定时器通道

#define RCU_IN2_TIMER          	RCU_TIMER11
#define BSP_IN2_TIMER         	TIMER11        // 定时器
#define BSP_IN2_CHANNEL       	TIMER_CH_1     // 定时器通道

#define RCU_IN3_TIMER         	RCU_TIMER2
#define BSP_IN3_TIMER        	TIMER2         // 定时器
#define BSP_IN3_CHANNEL        	TIMER_CH_3     // 定时器通道

#define RCU_IN4_TIMER         	RCU_TIMER2
#define BSP_IN4_TIMER         	TIMER2         // 定时器
#define BSP_IN4_CHANNEL       	TIMER_CH_2     // 定时器通道


#define IN1_OUT(X)  gpio_bit_write(PORT_IN1, GPIO_IN1, X?SET:RESET)
#define IN2_OUT(X)  gpio_bit_write(PORT_IN2, GPIO_IN2, X?SET:RESET)

#define IN3_OUT(X)  gpio_bit_write(PORT_IN3, GPIO_BIN3, X?SET:RESET)
#define IN4_OUT(X)  gpio_bit_write(PORT_IN4, GPIO_BIN4, X?SET:RESET)


void L298N_Init(uint16_t pre,uint16_t per);
void AO_Control(uint8_t dir, uint32_t speed);
void BO_Control(uint8_t dir, uint32_t speed);
#endif  /* BSP_L298N_H */


