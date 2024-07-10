#ifndef	_LIN_H
#define _LIN_H
#include "gd32f4xx.h"
#include "board.h"

/* LIN0 */
#define LIN0_PORT_TX         GPIOD
#define LIN0_PIN_TX          GPIO_PIN_5
#define LIN0_CLK_TX          RCU_GPIOD
#define LIN0_AF_TX           GPIO_AF_7

#define LIN0_PERIPH          USART1
#define LIN0_CLK_PERIPH      RCU_USART1
#define LIN0_IRQn            USART1_IRQn
#define LIN0_IRQHandler      USART1_IRQHandler
           
#define LIN0_BAUDRATE        19200
#define LIN_DATA_SIZE					7

typedef struct
{
	uint8_t idle;		//空,指的是同步间隔段，无需设置该成员
	uint8_t sync;		//同步段,默认是0x55;
	uint8_t pid;		//受保护ID段
	uint8_t data[LIN_DATA_SIZE];//数据段
	uint8_t checksum;//校验段，这里用的是增强型校验
}lin_pack_stu;

void lin_init(void);
uint8_t lin_checksum_calculate(uint8_t id, uint8_t* data, uint8_t data_len);
void lin_send_pack(lin_pack_stu* lin_pack);

#endif