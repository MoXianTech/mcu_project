/*************************************************************************
	> File Name: ble_type_E104.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/1/28 14:40:01
 ************************************************************************/

#ifndef __BLE_TYPE_H__
#define __BLE_TYPE_H__

#define BLE_LINK_GET \
    gpio_input_bit_get(GPIOB, GPIO_PIN_3)

void E104_bt5032A_init(USART_COM_ID_T com);

#endif
