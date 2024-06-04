#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"


#define GREEN_LED(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_14);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_14)						
						

//Co_Powered_PINS
#define R1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_12)	
#define R2(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_13);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_13)	
#define R3(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_11)					
#define R4(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_10);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_10)
#define R5(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)					
#define R6(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_1)
#define R7(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_0);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_0)				
#define R8(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_9);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_9)	
#define R9(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_8)
#define R10(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_7);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_7)					


#define AX_NUM 10
#define AY_NUM 5				
typedef struct 
{
   uint16_t value[AX_NUM][AY_NUM];
	 uint16_t sumAX[AY_NUM];
	 uint16_t voltage[AX_NUM][AY_NUM];
} Hc4051AdcValue;

uint8_t GetSwStatus(void);
void Hc4051IoInit(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value);
void ChooseEnlarge(uint8_t data);
void ProcessADCvalue(Hc4051AdcValue *adc_raw_value);
#endif
