#include "BLE.h"
#include "uart.h"
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "stdio.h"


//usart2_rx_t usart2_recv;

//void ble_init(void)
//{
//	usart2_init(9600);
//	char *cmd[9] = {	 "AT + AINT = 32",
//										 "AT + CINT = 6,12",
//										 "AT + CTOUT = 100",
//										 "AT + LUUID = 1111",
//										 "AT + BNAME = MX_BLE_SENSROR_001",
//										 "AT + NAME = MX_SPP_SENSOR",
//										 "AT + PIN = 6666",
//										 "AT + BAUD = 460800"};
//	
//	usart_dma_send_data(USART_2_TR, (uint8_t*)cmd[8], strlen(cmd[8]));
//	usart2_init(460800);
//										 
//	for(uint8_t i = 0; i < 8;)
//	{
//		usart_dma_send_data(USART_2_TR, (uint8_t*)cmd[i], strlen(cmd[i]));
//		if(1) //receive ok
//		{
//			i++;
//		}
//	}
//										 									 
//}

//void Board_BLE_data_send(uint8_t *data, uint32_t len)
//{
//		uint32_t i = 0;
//		if(gpio_input_bit_get(BT_STA_PORT,BT_STA_PIN) == 0)
//		{
//		/* transmit data */
//			for(i = 0; i < len; i++)
//			{
//				usart_data_transmit(USART2,*(data + i));
//				while(RESET == usart_flag_get(USART2,USART_FLAG_TBE));
//			}
//		}
//}

//void usart2_rx_judge(void)
//{
//	uint16_t cache_len = 0;
//	if(!usart_rx_probe(USART_2_TR))
//	{
//		memset(usart2_recv.recv_cache, 0x00, CACHE_BUFFER_SIZE);
//		cache_len = usart_get_rx_data_count(USART_2_TR);
//		usart_recv(USART_2_TR, usart2_recv.recv_cache, cache_len);
//		for(uint8_t index = 0; index < cache_len; index++)
//		{
//						
//		}
//	}
//}

