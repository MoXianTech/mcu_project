#ifndef _CAN_h
#define _CAN_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "uart.h"
void can0_transmit_Init(void);
void CAN0_Send_Message(uint32_t id, uint8_t *msg);
void communication_check(void);



#endif

