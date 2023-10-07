
#ifndef __BLT_h
#define __BLT_h

#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"

#define BT_STA_PORT							GPIOB
#define BT_STA_PIN							GPIO_PIN_9 


#define BT_KEY_PORT							GPIOB
#define BT_KEY_PIN							GPIO_PIN_8




#define BT_KEY(a)	if (a)	\
				gpio_bit_set(BT_KEY_PORT, BT_KEY_PIN);\
				else		\
				gpio_bit_reset(BT_KEY_PORT, BT_KEY_PIN)		
				
#define CACHE_BUFFER_SIZE 512				
typedef struct {
    uint8_t recv_cache[CACHE_BUFFER_SIZE];
} usart2_rx_t;

void BLT_Init(void);
void BLT_GPIO_Init(void);
void Board_BLE_data_send(uint8_t *data, uint32_t len);

#endif

