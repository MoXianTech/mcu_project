/*!
  \file  main.c
  \brief ADC0 regular channel with DMA

  \version 2017-12-26, V1.0.0, firmware for GD32E10x
  */

/*
   Copyright (c) 2017, GigaDevice Semiconductor Inc.

   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification, 
   are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.
   3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software without 
   specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
   OF SUCH DAMAGE.
   */
#include <stdlib.h>
#include "gd32e10x.h"
#include "systick.h"
#include <stdio.h>
#include "ws2812.h"
#include "uart.h"
#include "timer.h"
#include "adc.h"
#include "mx_serial_core.h"
#include "main.h"
#include "string.h"

#include "74HC4051BQ.h"
#include "pressure.h"
#include "resis.h"
#include "priv_malloc.h"
#include "pwm.h"
#include "vofa.h"
#include "differential_math.h"
#include "gt9147.h"
#include "soft_i2c.h"
#include "cs_press_driver.h"
/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

void check_date_running();
void motor_run();

bool moto_init_ok = true;
uint32_t time_1ms = 0;
int16_t pressure = 0;
uint32_t resis = 0;
uint8_t MX_buff[512];

mx_frame_rx_struct mx;
MyStruct motor;

Differential_t light_touch;
Differential_t heavy_touch;
mx_frame_rev_err get_MX_data(){
	uint16_t mrt_get_num = usart_get_rx_data_count(USART_0_TR);
	if(mrt_get_num > 0){
			
			uint16_t get_size = usart_recv(USART_0_TR, MX_buff, mrt_get_num);
			return	mx_frame_rev(MX_buff,mrt_get_num,&mx);
	}
	return SOF_ERR;
}
int mapped_value[12];
uint64_t time_count = 0;
int pwm_flag = 0;
int pwm_flag_t = 0;
int motor_state = 0;
uint8_t sensor_value[4];
uint8_t Last_sensor_value[4];
uint8_t temp11 = 0;
// 通过串口打印触摸点信息
void print_touch_info(void)
{
    uint8_t i;
    
    if(TPR_Structure.TouchSta & 0x1F) { // 检查是否有触摸点
        printf("Touch Points: %d\n", TPR_Structure.TouchSta & 0x0F);
        
        for(i = 0; i < 5; i++) {
            if(TPR_Structure.TouchSta & (1 << i)) {
                printf("Point %d: X=%d, Y=%d\n", i+1, TPR_Structure.x[i], TPR_Structure.y[i]);
            }
        }
    }
}
uint8_t a[10];
uint8_t buffer[30];
int ints[10];
uint8_t buffer2[30];
// 解析数据函数（示例）
bool parse_rawdata(unsigned char *raw, unsigned char len, CS_RAWDATA_Def *out) {
    if (len != AFE_USE_CH * 6 + 1) return false;
    
    unsigned char checksum = 0;
    for (int i = 0; i < AFE_USE_CH * 6; i++) checksum += raw[i];
    
    if (checksum != raw[AFE_USE_CH * 6]) return false;

    for (int i = 0; i < AFE_USE_CH; i++) {
        out->rawdata[i] = (raw[i*2+1] << 8) | raw[i*2];
        out->forcedata[i] = (raw[i*2+1 + 4*AFE_USE_CH] << 8) | raw[i*2 + 4*AFE_USE_CH];
    }
    return true;
}
void parse_buffer_to_ints(const uint8_t* buffer, int* output_ints, int num_ints) {
    for (int i = 0; i < num_ints; i++) {
        // 假设小端序（低位在前）
        output_ints[i] = (buffer[i*2 + 1] << 8) | buffer[i*2];
        
        // 如果是大端序（高位在前），则改为：
        // output_ints[i] = (buffer[i*2] << 8) | buffer[i*2 + 1];
    }
}
unsigned char rawdata[AFE_MAX_CH * 6 + 1];
unsigned char rawLen;
CS_RAWDATA_Def hx;
bool color_flag = 0;
int led_now = 0;
int color_counter = 0;
int main(void){
	systick_config();
	SystickInit();
	usart0_init(460800);
	pwm_init();
	adc_5init();
	delay_1ms(100);
	my_mem_init(SRAM_1);
	my_mem_init(SRAM_2);
	i2c_init();
	ws2812_process();
	ledAnimation();
	int pwm_flag = 0;
	
	cs_press_read_rawdata_init();
	char ret;
	differential_init(&light_touch,20,20,100,30,140,-10,5);
	//                    				周期，最小检测时间，最大检测时间，触发门槛，触发最高冲量，结束门槛,滤波周期

	while(1){
		motor_run();//电机驱动相关


		if(time_count%20 == 0){
			differential_run(&light_touch,return_adc_value());//20毫秒执行一次算法
			printf("%d\n",return_adc_value());
		}
		if(time_count%10 == 0){
			get_date_hx(buffer,buffer2);
			parse_buffer_to_ints(buffer, ints, 10);
			for (int i = 0; i < 10; i++) {
				if(ints[i]<=10000&&ints[i]>=7000){
					led_now = i;
					if(color_flag){
						setOneLEDColor(i,255,0,0);
						setSingleLEDColor(i-1,50,0,0);
						setSingleLEDColor(i+1,50,0,0);
					}
					else{
					setOneLEDColor(i,0,255,0);
					setSingleLEDColor(i-1,0,50,0);
					setSingleLEDColor(i+1,0,50,0);
					}
				}					
			}
			WS2812_Send();
		}
		time_count++;
		delay_1ms(1);
	}	
}
int motor_counter = 0;
void motor_run(){
//	static float motor_counter = 0;
//	motor = get_vofa_date();
	int motor_flag = check_button(&light_touch);
	
	if(motor_flag != 0){
		motor_state = 1;
		motor.run_strength = 2000;
		motor.run_time = 2;
		
	}
	if(motor.press == 1){
			motor_state = 1;
		}
	if(motor_state != 0){
		motor_counter ++;
		
		if(motor_state == 1){
			motor_counter = 0;
			motor_state = 2;
			pwm_flag = motor.run_strength;
			
		}
		if(motor_counter >= (int16_t)motor.run_time){
			motor_counter = 0;
			motor_state = 0;
			pwm_flag = 0;
			color_counter ++;
			if(color_counter%2 == 0){
				color_flag = !color_flag;
				if(color_flag){
						setOneLEDColor(led_now,255,0,0);
						setSingleLEDColor(led_now-1,50,0,0);
						setSingleLEDColor(led_now+1,50,0,0);
					}
				else{
					setOneLEDColor(led_now,0,255,0);
					setSingleLEDColor(led_now-1,0,50,0);
					setSingleLEDColor(led_now+1,0,50,0);
				}
			}
		}
	}
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_0, pwm_flag);
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_1, 4000 - pwm_flag);
	#if 1
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, pwm_flag);
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, 4000 - pwm_flag);
	#else
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 4000 - pwm_flag);
	timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, pwm_flag);
	#endif
	timer_enable(TIMER3);
}
void check_date_running(){
	output_mx_data(sensor_value);
	int all_data = 0;
	for (uint8_t i = 0; i < 4; i++) 
	{
		mapped_value[i] = 255 - sensor_value[i];
		if(mapped_value[i] < 5)
			mapped_value[i] = 5;
//			int filter_data = median_filter(i,mapped_value[i]);//开启滤波
		int  filter_data = mapped_value[i];
		all_data += filter_data;
		switch(i){
			case 0:
				mapped_value[4] = filter_data - Last_sensor_value[i];
				Last_sensor_value[i] = filter_data;
				setSingleLEDColor(0, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
				setSingleLEDColor(7, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
			break;
			case 1:
				mapped_value[5] = filter_data - Last_sensor_value[i];
				Last_sensor_value[i] = filter_data;
				setSingleLEDColor(5, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
				setSingleLEDColor(6, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
				break;
			case 2:
				mapped_value[6] = filter_data - Last_sensor_value[i];
				Last_sensor_value[i] = filter_data;
				setSingleLEDColor(1, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
				setSingleLEDColor(2, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
				break;
			case 3:
				mapped_value[7] = filter_data - Last_sensor_value[i];
				Last_sensor_value[i] = filter_data;
				setSingleLEDColor(3, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
				setSingleLEDColor(4, 0, filter_data, 0);  // 映射值做红绿亮度，蓝色为0
				break;
		}
		
	}	
	differential_run(&light_touch,all_data);
	mapped_value[8] = light_touch.current_input;
	mapped_value[9] = light_touch.delta;
	mapped_value[10] = light_touch.get_time;
	printf("samples:%d, %d, %d\n",mapped_value[8], mapped_value[9], mapped_value[10]);
//	printf("samples:%d, %d, %d, %d, %d, %d, %d, %d, %d\n",mapped_value[0], mapped_value[1], mapped_value[2], mapped_value[3], mapped_value[4], mapped_value[5], mapped_value[6], mapped_value[7],mapped_value[8]);
}

void TIMER3_IRQHandler( void ){
    if( SET == timer_interrupt_flag_get( TIMER3, TIMER_INT_FLAG_UP )){
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear( TIMER3, TIMER_INT_FLAG_UP );
    }
}



