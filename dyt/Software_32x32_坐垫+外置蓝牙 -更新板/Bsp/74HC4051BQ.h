#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"



#define GREEN_LED(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_15)						
						

// Column_Selection			
#define C_A(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_8)
#define C_B(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_11)
#define C_C(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_12)
				
				
#define AX0(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_0);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_0)				
#define AX1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_1)
#define AX2(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)
#define AX3(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_3);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_3)
#define AX4(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_4)
#define AX5(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_5)
#define AX6(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_6);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_6)
#define AX7(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_7);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_7)
#define AX8(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_8)	
#define AX9(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_9);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_9)	
#define AX10(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_10);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_10)
#define AX11(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_11)
#define AX12(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_12)
#define AX13(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_13);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_13)
#define AX14(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_14);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_14)
#define AX15(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_15)				

#define AX16(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_0);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_0)				
#define AX17(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_1)
#define AX18(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_2)
#define AX19(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_3);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_3)
#define AX20(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_4)
#define AX21(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_5)
#define AX22(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_6);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_6)
#define AX23(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_7);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_7)
#define AX24(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_8)	
#define AX25(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_9);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_9)	
#define AX26(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_10);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_10)
#define AX27(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_11)
#define AX28(a)	if (a)	\
				gpio_bit_set(GPIOC, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOC, GPIO_PIN_12)
#define AX29(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_6);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_6)
#define AX30(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_7);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_7)
#define AX31(a)	if (a)	\
				gpio_bit_set(GPIOD, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOD, GPIO_PIN_2)	

				
				

#define AX_NUM 32
#define AY_NUM 32				
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
