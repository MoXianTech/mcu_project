#include "pressure.h"
#include "uart.h"


void get_pressure(){
	static uint8_t motor_res[8];
	motor_res[0] = 0x01;
	motor_res[1] = 0x03;
	motor_res[2] = 0x00;
	motor_res[3] = 0x00;
	motor_res[4] = 0x00;
	motor_res[5] = 0x01;
	motor_res[6] = 0x84;
	motor_res[7] = 0x0A;
	usart_dma_send_data(USART_1_TR,(uint8_t *)&motor_res,sizeof(motor_res));
}

void pressure_init(){
	static uint8_t motor_res[8];
	motor_res[0] = 0x01;
	motor_res[1] = 0x06;
	motor_res[2] = 0x00;
	motor_res[3] = 0x11;
	motor_res[4] = 0x00;
	motor_res[5] = 0x01;
	motor_res[6] = 0x18;
	motor_res[7] = 0x0F;
	usart_dma_send_data(USART_1_TR,(uint8_t *)&motor_res,sizeof(motor_res));
	delay_1ms(100);
}
float timeabc = 0;
int16_t return_pressure(){
	static uint8_t pressure_buff[32];
	static int16_t pressure = 0;
	memset(pressure_buff,0,32);
	int cbuff_state = usart_recv(USART_1_TR, pressure_buff, 32);
	static uint64_t time = 0;
	if(cbuff_state){
		timeabc = GetSysRunTime() - time;
		time = GetSysRunTime();
		if(pressure_buff[0] == 0x01){
				if(pressure_buff[1] == 0x03){
					pressure = pressure_buff[3]<<8 | pressure_buff[4];
//					pressure = pressure * 0.1f;
					
				}
			}
		
	}
	
	return pressure;
}




