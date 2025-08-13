#include "vofa.h"
#include "uart.h"
uint8_t	uart_buff[64];

MyStruct motor_test;

float run_time = 0;
float run_up = 0;
float run_start = 0;
MyStruct get_vofa_date(){
	uint16_t uart_get_num = usart_get_rx_data_count(USART_0_TR);
	uint32_t raw_data = 0;
	motor_test.press = 0;
		if(uart_get_num > 0){
				uint16_t get_size = usart_recv(USART_0_TR, uart_buff, uart_get_num);
				if(uart_buff[0] == 0x5a && uart_buff[1] == 0xa5){
					switch(uart_buff[2]){
						case 0x01:
						  raw_data = (uart_buff[6] << 24) | (uart_buff[5] << 16) | (uart_buff[4] << 8) | uart_buff[3];
							motor_test.run_time = *(float*)&raw_data;
							break;
						case 0x02:
							raw_data = (uart_buff[6] << 24) | (uart_buff[5] << 16) | (uart_buff[4] << 8) | uart_buff[3];
							motor_test.run_strength = *(float*)&raw_data;
							break;
						case 0x03:
							motor_test.press = 1;
							break;
					}
				}
		}
		return motor_test;
}
