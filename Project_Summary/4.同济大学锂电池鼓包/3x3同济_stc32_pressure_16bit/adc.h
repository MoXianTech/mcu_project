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

#define	ADC_CH		1			/* 1~16, ADCת��ͨ����, ��ͬ���޸�ת��ͨ�� */
#define	ADC_DATA	12			/* 6~n, ÿ��ͨ��ADCת����������, 2*ת������+4, ��ͬ���޸�ת������ */

#define AY_NUM 10



#define CS0(a)   P12 = a;
#define CS1(a)   P13 = a;
#define CS2(a)   P17 = a;
#define CS3(a)   P16 = a;

typedef struct
{
	u16 raw_value[AY_NUM];
	u16 p_value[AY_NUM];
} Adc_value;

void ADC_DMA_Init(void);
void read_adc_value(void);
u16 GetAdcValue(u8 index);

void AY_scan(u8 index);
void Get_Voltage_Value(Adc_value *process_adc_value);

#endif




