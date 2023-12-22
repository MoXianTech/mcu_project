#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"



#define GREEN_LED(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)						
						

// Column_Selection			
#define C_A(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_15)
#define C_B(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_14);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_14)
#define C_C(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_13);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_13)

#define AX_NUM 32
#define AY_NUM 64				
/*32x64 Data_Array*/	
typedef struct 
{
   uint16_t value[AX_NUM][AY_NUM];
	 uint16_t sumAX[AY_NUM];
	 uint8_t voltage[AX_NUM][AY_NUM];

} Hc4051AdcValue;

uint8_t GetSwStatus(void);
void Hc4051IoInit(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value);
void ChooseEnlarge(uint8_t data);
void ProcessADCvalue(Hc4051AdcValue *adc_raw_value);
#endif
