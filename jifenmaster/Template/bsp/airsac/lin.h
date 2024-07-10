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
	uint8_t idle;		//��,ָ����ͬ������Σ��������øó�Ա
	uint8_t sync;		//ͬ����,Ĭ����0x55;
	uint8_t pid;		//�ܱ���ID��
	uint8_t data[LIN_DATA_SIZE];//���ݶ�
	uint8_t checksum;//У��Σ������õ�����ǿ��У��
}lin_pack_stu;

void lin_init(void);
uint8_t lin_checksum_calculate(uint8_t id, uint8_t* data, uint8_t data_len);
void lin_send_pack(lin_pack_stu* lin_pack);

#endif