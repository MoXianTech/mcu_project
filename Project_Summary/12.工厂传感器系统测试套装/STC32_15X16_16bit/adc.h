#ifndef __ADC_H
#define __ADC_H

#include	"config.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_ADC.h"
#include	"STC32G_UART.h"
#include	"STC32G_DMA.h"
#include	"STC32G_NVIC.h"
#include	"STC32G_Delay.h"
#include	"STC32G_Switch.h"
#include 	"stdio.h"

#define	ADC_CH		1			/* 1~16, ADC转换通道数, 需同步修改转换通道 */
#define	ADC_DATA	12			/* 6~n, 每个通道ADC转换数据总数, 2*转换次数+4, 需同步修改转换次数 */
#define AX_NUM 16
#define AY_NUM 16



#define AX0(a)   P32 = a;  
#define AX1(a)   P33 = a;
#define AX2(a)   P34 = a;  
#define AX3(a)   P35 = a; 
#define AX4(a)   P36 = a;
#define AX5(a) 	 P37 = a;
#define AX6(a)   P20 = a;
#define AX7(a)   P21 = a;
#define AX8(a)   P22 = a;
#define AX9(a)   P23 = a;
#define AX10(a)  P24 = a;
#define AX11(a)  P25 = a;
#define AX12(a)  P26 = a;
#define AX13(a)  P27 = a;
#define AX14(a)  P00 = a;
#define AX15(a)  P01 = a;

#define CS0(a)   P16 = a;
#define CS1(a)   P17 = a;
#define CS2(a)   P13 = a;
#define CS3(a)   P12 = a;

typedef struct
{
	u16 raw_value[AX_NUM][AY_NUM];
	u16 processed_adc_value[AX_NUM][AY_NUM];
	u16 sumAX[AY_NUM];
} Adc_value;

void ADC_DMA_Init(void);
void read_adc_value(void);
u16 GetAdcValue(u8 index);
void AX_scan(u8 index);
void AY_scan(u8 index);
void Get_Voltage_Value(Adc_value *process_adc_value, u8 count_ay);
void ProcessADCvalue(Adc_value *process_adc_value);
#endif




