#include "BLT.h"
#include "gd32e10x.h"
#include "uart.h"



void BLT_GPIO_Init(void)
{
		rcu_periph_clock_enable(RCU_GPIOD);		
	  gpio_init(BT_STA_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, BT_STA_PIN); //BLT_LINK -> STATE 
}

void BLT_Init(void)
{
		BLT_GPIO_Init();	
		usart0_init(9600);
		char *cmd[4] = {	 "AT+BNAME=MX_BLE_SENSOR_001",									 
											 "AT+PIN=6666",
											 "AT+NAME=MXSPP_SENSOR",
											 "AT+BAUD=460800"};
		usart_dma_send_data(USART_0_TR, (uint8_t*)cmd[3], strlen(cmd[3]));
		delay_1ms(1000);
		usart0_init(460800);
	
		for(uint8_t cmd_num = 0; cmd_num < 3; cmd_num++)
		{
			usart_dma_send_data(USART_0_TR, (uint8_t *)cmd[cmd_num], strlen(cmd[cmd_num]));
			delay_1ms(1200);
			}
}	





