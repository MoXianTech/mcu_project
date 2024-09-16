#include "BLE.h"
#include "uart.h"
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "stdio.h"


usart2_rx_t usart2_recv;

void ble_gpio_init(void)
{
	  rcu_periph_clock_enable(RCU_GPIOC);
//		gpio_init(BT_KEY_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, BT_KEY_PIN | BT_RESET_PIN);
//		gpio_init(BT_RESET_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,  BT_RESET_PIN);
		gpio_init(BT_STA_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, BT_STA_PIN);
//		BT_KEY(1); //1 ->transsmition 0->configuration
//		BT_RESET(0);

}

void ble_init(void)
{
		usart2_init(9600);
		ble_gpio_init();

		char *cmd[5] = {	 "AT+BNAME=MX_BLE_SENSROR_001",
											 "AT+NAME=MXSPP_SENSOR",
											 "AT+PIN=6666",
											 "AT+BAUD=460800"};
		
		usart_dma_send_data(USART_2_TR, (uint8_t *)cmd[3], strlen(cmd[3]));
		delay_1ms(1000);
		usart2_init(460800);
										 
		for(uint8_t i = 0; i < 3; i++)
		{
			usart_dma_send_data(USART_2_TR, (uint8_t*)cmd[i], strlen(cmd[i]));
			delay_1ms(1200);
		}	

//		BT_KEY(1); //1 ->transsmition 0->configuration
//		BT_RESET(0);		
}

void Board_BLE_data_send(uint8_t *data, uint32_t len)
{
		uint32_t i = 0;
//		BT_KEY(1); //1 ->transsmition 0->configuration
		if(gpio_input_bit_get(BT_STA_PORT,BT_STA_PIN) == 0)
		{
			usart_dma_send_data(USART_2_TR,data,len);
		}
}

//unsigned int usart2_rx_judge(void)
//{
//		uint16_t cache_len = 0;

//		
//		if(!usart_rx_probe(USART_2_TR))
//		{
//				memset(usart2_recv.recv_cache, 0x00, CACHE_BUFFER_SIZE);
//				cache_len = usart_get_rx_data_count(USART_2_TR);
//				usart_recv(USART_2_TR, usart2_recv.recv_cache, cache_len);
//			
//				for(uint8_t index = 0; index < cache_len; index++)
//				{
//					if((char*)(usart2_recv.recv_cache + index) == "O" && (char*)(usart2_recv.recv_cache + index + 1) == "K")
//					{
//						return 1;
//					}		
//					else
//					{
//						return 0;
//					}
//				}
//		}
//}

