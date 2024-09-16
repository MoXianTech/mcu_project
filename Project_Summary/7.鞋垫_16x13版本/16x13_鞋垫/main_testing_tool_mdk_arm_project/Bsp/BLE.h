#ifndef __BLE_h
#define __BLE_h


#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"

#define BT_STA_PORT							GPIOC
#define BT_STA_PIN							GPIO_PIN_13 

#define BT_RESET_PORT						GPIOC
#define BT_RESET_PIN						GPIO_PIN_14

#define BT_KEY_PORT							GPIOC
#define BT_KEY_PIN							GPIO_PIN_15

#define BT_RESET(a)	if (a)	\
				gpio_bit_set(BT_RESET_PORT, BT_RESET_PIN);\
				else		\
				gpio_bit_reset(BT_RESET_PORT, BT_RESET_PIN)

#define BT_KEY(a)	if (a)	\
				gpio_bit_set(BT_KEY_PORT, BT_KEY_PIN);\
				else		\
				gpio_bit_reset(BT_KEY_PORT, BT_KEY_PIN)

#define CACHE_BUFFER_SIZE 512				
typedef struct {
    uint8_t recv_cache[CACHE_BUFFER_SIZE];
} usart2_rx_t;


void ble_gpio_init(void);
void ble_init(void);
void Board_BLE_data_send(uint8_t *data, uint32_t len);
//unsigned int usart2_rx_judge(void);
#endif

