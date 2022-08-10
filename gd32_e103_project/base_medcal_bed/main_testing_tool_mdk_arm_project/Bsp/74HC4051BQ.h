#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
#include "flash.h"

#define RED_LED(a)	if (a)	\
							gpio_bit_set(GPIOB, GPIO_PIN_14);\
						else		\
							gpio_bit_reset(GPIOB, GPIO_PIN_14)
#define GREEN_LED(a)	if (a)	\
							gpio_bit_set(GPIOB, GPIO_PIN_15);\
						else		\
							gpio_bit_reset(GPIOB, GPIO_PIN_15)						
						
// MA MB 分压电阻选择
#define MA(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_3);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_3)
					
#define MB(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_4);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_4)	


// 行选	R1~R16			
#define CA(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_5);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_5)
#define CB(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_8);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_8)

//列选	C1~C16			
				
#define A1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_0);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_0)	

#define B1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_1);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_1)

#define C1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_2);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_2)
					
#define EN1(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_10);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_10)	

#define EN2(a)	if (a)	\
				gpio_bit_set(GPIOB, GPIO_PIN_11);\
				else		\
				gpio_bit_reset(GPIOB, GPIO_PIN_11)				

				
typedef struct 
{
    uint16_t value[16][16];
} Hc4051AdcValue;

uint8_t GetSwStatus(void);
void Hc4051IoInit(void);
Hc4051AdcValue ReadAdcValue(void);
void ChooseEnlarge(uint8_t data);
#endif
