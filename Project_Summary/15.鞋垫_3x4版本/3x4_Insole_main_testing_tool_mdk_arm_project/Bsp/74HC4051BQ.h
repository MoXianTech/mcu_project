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
						
// MA MB 分压电阻选择
#define MA(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_9);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_9)
					
#define MB(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_8)	

// 行选	R1~R16			
#define CA(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_5)
#define CB(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_6);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_6)

// Column Select
#define AX1(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_4)	
#define AX2(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_5)
#define AX3(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_6);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_6)			
#define AX4(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_7);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_7)
#define AX5(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_0);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_0)				
#define AX6(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_1)							
#define AX7(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)
#define AX8(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_4)
#define AX9(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_3);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_3)	
#define AX10(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_12)
#define AX11(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_11)
#define AX12(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_8)		
#define AX13(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_15)				
#define AX14(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_14);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_14)					
#define AX15(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_13);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_13)
#define AX16(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_12)				
				
#define AX_NUM 16
#define AY_NUM 16
			
typedef struct 
{
   uint16_t value[AX_NUM][AY_NUM];
	 uint16_t sumAX[AY_NUM];
	 uint16_t voltage[AX_NUM][AY_NUM];
	 uint16_t d_voltage[AX_NUM][AY_NUM];
	 uint16_t voltage_display[AX_NUM][AY_NUM];
	 uint8_t  insole_display[6][2];
} Hc4051AdcValue;

uint8_t GetSwStatus(void);
void Hc4051IoInit(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value);
void ProcessADCvalue(Hc4051AdcValue *adc_raw_value);
void display_reampping(Hc4051AdcValue *adc_raw_value);
void ChooseEnlarge(uint8_t data);
#endif
