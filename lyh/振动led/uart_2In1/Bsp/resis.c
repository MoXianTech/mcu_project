#include "resis.h"
#include "uart.h"
union MIX_pressure_resis_Command_t mix_data;
uint8_t resis_buff[32];
uint32_t return_resis(void){
	
	static uint32_t resis = 0;
	memset(resis_buff,0,32);
	int cbuff_state = usart_recv(USART_2_TR, resis_buff, 32);
	if(cbuff_state){
		if(resis_buff[0] == 0xa5){
				if(resis_buff[1] == 0x5a){
					resis = resis_buff[2]<<24 | resis_buff[3]<<16 | resis_buff[4]<<8 | resis_buff[5];
				}
			}
		
	}
	return resis;
}

void transmit_mix_data(int16_t pressure,uint32_t resis){
	mix_data.MIXCmd.pressure = pressure;
	mix_data.MIXCmd.resis = resis;
	usart_dma_send_data(USART_0_TR,(uint8_t *)&mix_data.data,sizeof(mix_data.data));
}

void mix_date_init(void){
	mix_data.MIXCmd.Head1 = 0xa5;
	mix_data.MIXCmd.Head2 = 0x5a;
	mix_data.MIXCmd.cmd = 0x55;
	mix_data.MIXCmd.tail = 0xaa;
}