#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"


#define GREEN_LED(a)	if (a)	\
							gpio_bit_set(GPIOA, GPIO_PIN_3);\
						else		\
							gpio_bit_reset(GPIOA, GPIO_PIN_3)						
						
// MA MB ·ÖÑ¹µç×èÑ¡Ôñ
#define MA(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_1)
					
#define MB(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_2)	



// Column Select Maping
#define C1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_9);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_9)	
#define C2(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_8)
#define C3(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_7);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_7)			
#define C4(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_6);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_6)
#define C5(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_5)				
#define C6(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_4)							
#define C7(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_3);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_3)
#define C8(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_15)
#define C9(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_12)	
#define C10(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_11)
#define C11(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_8)
#define C12(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_15);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_15)		
#define C13(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_14);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_14)				
#define C14(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_13);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_13)					
#define C15(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_12);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_12)
#define C16(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_11)			
				
#define C17(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_10);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_10)						
#define C18(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)
#define C19(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_1)
#define C20(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_0);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_0)
#define C21(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_7);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_7)
#define C22(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_6);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_6)
#define C23(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_5)
#define C24(a)	if (a)	\
				gpio_bit_set(GPIOA, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOA, GPIO_PIN_4)
				
							
typedef struct 
{
   uint16_t value[24][1];
} Hc4051AdcValue;

uint8_t GetSwStatus(void);
void Hc4051IoInit(void);
void ReadAdcValue(Hc4051AdcValue *adc_raw_value);
void ChooseEnlarge(uint8_t data);
#endif
