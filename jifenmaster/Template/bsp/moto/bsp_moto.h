#ifndef _BSP_MOTO_H
#define _BSP_MOTO_H
#include "gd32f4xx.h"
#include "board.h"

#define RCU_IN1                 RCU_GPIOD
#define PORT_IN1                GPIOD
#define GPIO_IN1                GPIO_PIN_14
#define AF_IN1                  GPIO_AF_2

#define RCU_IN2                 RCU_GPIOD
#define PORT_IN2                GPIOD
#define GPIO_IN2                GPIO_PIN_15
#define AF_IN2                  GPIO_AF_2

#define RCU_IN3                 RCU_GPIOD
#define PORT_IN3                GPIOD
#define GPIO_IN3                GPIO_PIN_12
#define AF_IN3                  GPIO_AF_2

#define RCU_IN4                 RCU_GPIOD
#define PORT_IN4                GPIOD
#define GPIO_IN4                GPIO_PIN_13
#define AF_IN4                  GPIO_AF_2


#define RCU_IN5                 RCU_GPIOB
#define PORT_IN5                GPIOB
#define GPIO_IN5                GPIO_PIN_10
#define AF_IN5                  GPIO_AF_1

#define RCU_IN6                 RCU_GPIOB
#define PORT_IN6                GPIOB
#define GPIO_IN6                GPIO_PIN_11
#define AF_IN6                  GPIO_AF_1

#define RCU_IN7                 RCU_GPIOB
#define PORT_IN7                GPIOB
#define GPIO_IN7                GPIO_PIN_8
#define AF_IN7                  GPIO_AF_1

#define RCU_IN8                 RCU_GPIOB
#define PORT_IN8                GPIOB
#define GPIO_IN8                GPIO_PIN_9
#define AF_IN8                  GPIO_AF_1

#define RCU_IN1_TIMER        	RCU_TIMER3
#define BSP_IN1_TIMER        	TIMER3        // 定时器
#define BSP_IN1_CHANNEL        	TIMER_CH_2     // 定时器通道

#define RCU_IN2_TIMER          	RCU_TIMER3
#define BSP_IN2_TIMER         	TIMER3        // 定时器
#define BSP_IN2_CHANNEL       	TIMER_CH_3     // 定时器通道

#define RCU_IN3_TIMER         	RCU_TIMER3
#define BSP_IN3_TIMER        	TIMER3         // 定时器
#define BSP_IN3_CHANNEL        	TIMER_CH_0     // 定时器通道

#define RCU_IN4_TIMER         	RCU_TIMER3
#define BSP_IN4_TIMER         	TIMER3         // 定时器
#define BSP_IN4_CHANNEL       	TIMER_CH_1     // 定时器通道

#define RCU_IN5_TIMER        	RCU_TIMER1
#define BSP_IN5_TIMER        	TIMER1        // 定时器
#define BSP_IN5_CHANNEL        	TIMER_CH_2     // 定时器通道

#define RCU_IN6_TIMER          	RCU_TIMER1
#define BSP_IN6_TIMER         	TIMER1        // 定时器
#define BSP_IN6_CHANNEL       	TIMER_CH_3     // 定时器通道

#define RCU_IN7_TIMER         	RCU_TIMER1
#define BSP_IN7_TIMER        	TIMER1         // 定时器
#define BSP_IN7_CHANNEL        	TIMER_CH_0     // 定时器通道

#define RCU_IN8_TIMER         	RCU_TIMER1
#define BSP_IN8_TIMER         	TIMER1         // 定时器
#define BSP_IN8_CHANNEL       	TIMER_CH_1     // 定时器通道



#define IN1_OUT(X)  gpio_bit_write(PORT_IN1, GPIO_IN1, X?SET:RESET)
#define IN2_OUT(X)  gpio_bit_write(PORT_IN2, GPIO_IN2, X?SET:RESET)

#define IN3_OUT(X)  gpio_bit_write(PORT_IN3, GPIO_IN3, X?SET:RESET)
#define IN4_OUT(X)  gpio_bit_write(PORT_IN4, GPIO_IN4, X?SET:RESET)

#define IN5_OUT(X)	gpio_bit_write(PORT_IN5, GPIO_IN5, X?SET:RESET)
#define IN6_OUT(X)	gpio_bit_write(PORT_IN6, GPIO_IN6, X?SET:RESET)

#define IN7_OUT(X)	gpio_bit_write(PORT_IN7, GPIO_IN7, X?SET:RESET)
#define IN8_OUT(X)	gpio_bit_write(PORT_IN8, GPIO_IN8, X?SET:RESET)

typedef enum
{
    MOTO_1,
    MOTO_2,
    MOTO_3,
    MOTO_4
} MOTO_NUM;

extern volatile uint8_t moto_dire_flag;

void moto_init(uint16_t pre,uint16_t per);
void AO_Control(uint8_t dir, uint32_t speed);
void BO_Control(uint8_t dir, uint32_t speed);
void CO_Control(uint8_t dir, uint32_t speed);
void DO_Control(uint8_t dir, uint32_t speed);
void set_moto_control(MOTO_NUM moto_num, int16_t speed);

#endif
