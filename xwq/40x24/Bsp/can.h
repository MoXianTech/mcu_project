#ifndef __CAN_h
#define __CAN_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#define DEV_CAN0_ID          (0x300)

typedef enum
{
	CAN_DISABLE,
	CAN_ONCE_TIGGER,
	CAN_CONTINUE
}Enum_can_mode;

extern volatile Enum_can_mode can0_receive_fifo0_flag;

void can0_init(void);
void read_can0_data(can_receive_message_struct *receive_message);
void send_can0_data(uint32_t id, uint8_t *msg, uint8_t msg_len);
void send_can0_buffer(uint32_t id, uint8_t *buffer, uint16_t buffer_len);
void can0_rx0_interrupt_handler(void);

#endif

