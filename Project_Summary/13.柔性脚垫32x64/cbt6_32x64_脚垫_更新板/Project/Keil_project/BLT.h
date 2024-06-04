
#ifndef __BLT_h
#define __BLT_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"

#define BT_STA_PORT							GPIOD
#define BT_STA_PIN							GPIO_PIN_2 




			

void BLT_Init(void);
void BLT_GPIO_Init(void);
void Board_BLE_data_send(uint8_t *data, uint32_t len);

#endif

