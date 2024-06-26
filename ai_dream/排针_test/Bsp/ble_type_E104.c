/*************************************************************************
  > File Name: ble_type_E104.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: 2023/1/28 14:39:45
 ************************************************************************/

#include "uart.h"
#include "ble_type_E104.h"
#include "at_commit_base.h"

#define CMD_LEN_E104 64

#define BLE_LINK_GET \
    gpio_input_bit_get(GPIOB, GPIO_PIN_3)

#define BLE_MODE_AT \
    gpio_bit_reset(GPIOB, GPIO_PIN_5)

#define BLE_MODE_UART \
    gpio_bit_set(GPIOB, GPIO_PIN_5)

#define BLE_RESET_E104 \
    gpio_bit_reset(GPIOC, GPIO_PIN_12);\
    delay_1ms(500);\
    gpio_bit_set(GPIOC, GPIO_PIN_12);

void E104_bt5032A_init(USART_COM_ID_T com)
{
    char cmd_recv[CMD_LEN_E104] = {0};
    char cmd_send[CMD_LEN_E104] = {0};
    
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_12);
    delay_1ms(500);
		
    BLE_RESET_E104;
    BLE_MODE_AT;
    delay_1ms(500);
    at_config_at_cmd_timeout(com, "at+reset", NULL, 0, "+OK", 200);
    delay_1ms(500);
    at_consult_at_cmd(com, "at+mac?", cmd_recv, CMD_LEN_E104, "+OK=");
    reverse(cmd_recv);
    strcat(cmd_send, "at+advdat=");
    strcat(cmd_send, cmd_recv);
    at_config_at_cmd_timeout(com, cmd_send, NULL, 0, "+OK", 200);
    at_config_at_cmd_timeout(com, "at+reset", NULL, 0, "+OK", 0);
    delay_1ms(500);

    BLE_MODE_UART;
}

