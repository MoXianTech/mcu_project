#include "74hc4051bq.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>
#include "i2c_XL9555.h"

Hc4051AdcValue adc_raw_value;
#define ADC_DELAY_TIME (2 * 220) //9 ns * 120

void Hc4051Delay(uint32_t num)
{
    while(num --);
}

/*
功能：Init
输入：none
*/ 

void led_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 ); //LED_G
	GREEN_LED(0);
	rcu_periph_clock_enable(RCU_AF);
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9 );
	gpio_bit_reset(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
}


void RS2251_Init(void)
{
		rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}

/*
功能：AY SCAN
输入：data 0 -> 7
*/   
void Choose_AY_Channel(uint8_t data)
{                                                                                            
    C_A( data & 0x01);                                              
    C_B((data & 0x02) >> 1);
		C_C((data & 0x04) >> 2);
}



/*
功能：行列扫描读取数据
输入：无
返回：返回32*64的数据
*/
void ReadAdcValue(Hc4051AdcValue *adc_raw_value,uint8_t ax_index)
{	
			uint8_t A_adc_rank[8] = {2,1,0,3,4,6,7,5};
			uint8_t B_adc_rank[8] = {4,6,7,5,2,1,0,3};

					for(uint8_t ay = 0; ay < 8; ++ay)
					{
	
						if(ax_index < 24)
						{
								Choose_AY_Channel(A_adc_rank[ay]);
								Hc4051Delay(ADC_DELAY_TIME);
								adc_raw_value->sensorA_raw_value[ax_index][ay]	    = GetAdcValue(1);
								adc_raw_value->sensorA_raw_value[ax_index][ay +  8]	= GetAdcValue(0);
								adc_raw_value->sensorA_raw_value[ax_index][ay + 16]	= GetAdcValue(3);
						}
						else
							
						{
								Choose_AY_Channel(B_adc_rank[ay]);	
								Hc4051Delay(ADC_DELAY_TIME);
								adc_raw_value->sensorB_raw_value[ax_index - 24][ay]	      = GetAdcValue(5);
								adc_raw_value->sensorB_raw_value[ax_index - 24][ay +  8]	= GetAdcValue(4);
								adc_raw_value->sensorB_raw_value[ax_index - 24][ay + 16]	= GetAdcValue(2);
						}		
					}		
}


void ProcessADCvalue(Hc4051AdcValue *adc_raw_value, SENSOR_NAME_T sensor, uint16_t display_threshold_value)
{
		uint16_t Vref = 4095;
		uint16_t flag_sum = 1;

	switch(sensor)
	{
			case SENSOR_A:
			
						while(flag_sum)
						{					
							for(uint8_t AY = 0; AY < AY_NUM; ++AY)
							{
								for(uint8_t AX = 0; AX < AX_NUM; ++AX)
								{
										if(adc_raw_value->sensorA_raw_value[AX][AY] <= display_threshold_value)
										{
											adc_raw_value->sensorA_raw_value[AX][AY] = 0;
										}
										
										adc_raw_value->sumAX[AY] += adc_raw_value->sensorA_raw_value[AX][AY];
								}
							}	
							flag_sum = 0;						
						}
				
						while(flag_sum == 0)
						{
						
							for(uint8_t AY = 0; AY < AY_NUM; ++AY)
							{			
									for(uint8_t AX = 0; AX < AX_NUM; ++AX)
									{	
										if(adc_raw_value->sensorA_raw_value[AX][AY] > display_threshold_value)
										{									

											adc_raw_value->sensorA_voltage[AX][AY] =  (Vref * (uint32_t)adc_raw_value->sensorA_raw_value[AX][AY])
											/ (Vref - (adc_raw_value->sumAX[AY] - adc_raw_value->sensorA_raw_value[AX][AY]));	
											
											if(adc_raw_value->sensorA_voltage[AX][AY] > 65534)
											{
												adc_raw_value->sensorA_voltage[AX][AY] = 65534;
											}
										} else {
												adc_raw_value->sensorA_voltage[AX][AY] = 0;
										}
									}
							} 		
							memset(adc_raw_value->sumAX, 0 , sizeof(adc_raw_value->sumAX));	
							flag_sum = 1;
						}
		
				break;
				
			case SENSOR_B:
				
						while(flag_sum)
						{					
							for(uint8_t AY = 0; AY < AY_NUM; ++AY)
							{
								for(uint8_t AX = 0; AX < AX_NUM; ++AX)
								{
										if(adc_raw_value->sensorB_raw_value[AX][AY] <= display_threshold_value)
										{
											adc_raw_value->sensorB_raw_value[AX][AY] = 0;
										}
										adc_raw_value->sumAX[AY] += adc_raw_value->sensorB_raw_value[AX][AY];
								}
							}	
							flag_sum = 0;						
						}
				
						while(flag_sum == 0)
						{
						
							for(uint8_t AY = 0; AY < AY_NUM; ++AY)
							{			
									for(uint8_t AX = 0; AX < AX_NUM; ++AX)
									{	
										if(adc_raw_value->sensorB_raw_value[AX][AY] > display_threshold_value)
										{									

											adc_raw_value->sensorB_voltage[AX][AY] =  (adc_raw_value->sensorB_raw_value[AX][AY] * 255)
											/ (Vref - (adc_raw_value->sumAX[AY] - adc_raw_value->sensorB_raw_value[AX][AY]));									
											if(adc_raw_value->sensorB_voltage[AX][AY] > 65534)
											{
												adc_raw_value->sensorB_voltage[AX][AY] = 65534;
											}
										}
										else
										{
											adc_raw_value->sensorB_voltage[AX][AY] = 0;
										}
									}
							} 		
							memset(adc_raw_value->sumAX, 0 , sizeof(adc_raw_value->sumAX));	
							flag_sum = 1;
						}
						
				break;
						
			default:
				
			break;
			
	}

	

}

void display_remapping(Hc4051AdcValue *adc_raw_value)
{

	
}

