#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"



#define GREEN_LED(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_12)						
						

// Column_Selection			
#define C_A(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_13);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_13)
#define C_B(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_14);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_14)
#define C_C(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_15)

#define AX_NUM 24
#define AY_NUM 24				
/*32x64 Data_Array*/	
typedef struct 
{
   uint16_t sensorA_raw_value[AX_NUM][AY_NUM];
	 uint16_t sensorB_raw_value[AX_NUM][AY_NUM];
	 uint32_t sumAX[AY_NUM];
	
	 uint8_t sensorA_voltage[AX_NUM][AY_NUM];
	 uint8_t sensorB_voltage[AX_NUM][AY_NUM];
} Hc4051AdcValue;

typedef enum SENSOR_NAME
{
	SENSOR_A,
	SENSOR_B
} SENSOR_NAME_T;

uint8_t GetSwStatus(void);
void RS2251_Init(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value, uint8_t ax_index);
void ChooseEnlarge(uint8_t data);
void ProcessADCvalue(Hc4051AdcValue *adc_raw_value, SENSOR_NAME_T sensor, uint16_t display_threshold_value);
void display_remappading(Hc4051AdcValue *adc_raw_value);
void led_init(void);
#endif
