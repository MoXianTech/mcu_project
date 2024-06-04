#include  "adc.h"
#include	"config.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_ADC.h"
#include	"STC32G_UART.h"
#include	"STC32G_DMA.h"
#include	"STC32G_NVIC.h"
#include	"STC32G_Delay.h"
#include	"STC32G_Switch.h"
#include 	"stdio.h"
#include 	<string.h>
#include 	<math.h>


/*************	���ر�������	**************/

u16 xdata DmaAdBuffer[ADC_CH][ADC_DATA] = {0};
u16 adc_value[ADC_CH] = {0};
Adc_value processe_adc_value;


#define ADC_DELAY_TIME (50 * 42) //9 ns * 120

void Hc4051Delay(u32 num)
{
    while(num --);
}

/******************** IO������ ********************/
void	ADC_GPIO_config(void)
{
  P1_MODE_IN_HIZ(GPIO_Pin_0);		//P1.0~P1.7 ����Ϊ�������� adc
	P1_MODE_OUT_PP(GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_3 | GPIO_Pin_2); //16_selector
	P1_SPEED_HIGH(GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_3 | GPIO_Pin_2);
}


/******************** ADC ���� ********************/
void ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//�ṹ����

	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC ģ���źŲ���ʱ�����, 0~31��ע�⣺ SMPDUTY һ����������С�� 10��
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC ͨ��ѡ��ʱ����� 0(Ĭ��),1
	ADC_InitStructure.ADC_CsHold    = 1;		//ADC ͨ��ѡ�񱣳�ʱ����� 0,1(Ĭ��),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X2T;		//���� ADC ����ʱ��Ƶ��	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC�������,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_Inilize(&ADC_InitStructure);		//��ʼ��
	ADC_PowerControl(ENABLE);						//ADC��Դ����, ENABLE��DISABLE
	NVIC_ADC_Init(DISABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/******************** DMA ���� ********************/
void	ADC_DMA_config(void)
{
	DMA_ADC_InitTypeDef		DMA_ADC_InitStructure;		//�ṹ����

	DMA_ADC_InitStructure.DMA_Enable = ENABLE;			//DMAʹ��  	ENABLE,DISABLE
	DMA_ADC_InitStructure.DMA_Channel = 0x0001;			//ADCͨ��ʹ�ܼĴ���, 1:ʹ��, bit15~bit0 ��Ӧ ADC15~ADC0
	DMA_ADC_InitStructure.DMA_Buffer = (u16)DmaAdBuffer;	//ADCת�����ݴ洢��ַ
	DMA_ADC_InitStructure.DMA_Times = ADC_4_Times;	//ÿ��ͨ��ת������, ADC_1_Times,ADC_2_Times,ADC_4_Times,ADC_8_Times,ADC_16_Times,ADC_32_Times,ADC_64_Times,ADC_128_Times,ADC_256_Times
	DMA_ADC_Inilize(&DMA_ADC_InitStructure);		//��ʼ��
	NVIC_DMA_ADC_Init(ENABLE,Priority_0,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
	DMA_ADC_TRIG();		//��������ת��
}


void ADC_DMA_Init(void)
{
	ADC_GPIO_config();
	ADC_config();
	ADC_DMA_config();
}


u16 GetAdcValue(u8 index)
{
		if(index < ADC_CH)
		{
			return DmaAdBuffer[0][5]; //[i][j] ->i- channel_num / j - adc_4times [0]-[3]; [4]-channel_num; [5] -4times avergr
		}
		else
		{
			return 0;
		}	
}



void AY_scan(u8 index)
{			
			CS0(index  & 0x0001);
			CS1((index & 0x0002) >> 1);
			CS2((index & 0x0004) >> 2);
			CS3((index & 0x0008) >> 3);	
}



void Get_Voltage_Value(Adc_value *process_adc_value)
{
	u8 ay;
	u16 Vref = 4095;
	u16 Rr_value = 1000;	


	for(ay = 0; ay < AY_NUM; ay++)
	{
			AY_scan(ay);

			Hc4051Delay(ADC_DELAY_TIME);

			process_adc_value->raw_value[ay] = GetAdcValue(0);
	
			/*pressure_remap*/
		if(process_adc_value->raw_value[ay] <= 200)
		{
			process_adc_value->p_value[ay]  
				 = 0.0263 *(float) process_adc_value->raw_value[ay] - 0.3527;
			
		}else {
							process_adc_value->p_value[ay] 
										= 0.6 * exp(0.0022	* (float)process_adc_value->raw_value[ay]) + 4;	
		}

			if(process_adc_value->p_value[ay] <= 1)
			{
				process_adc_value->p_value[ay] = 0;
			}
			if(process_adc_value->p_value[ay] > 65535)
			{
				process_adc_value->p_value[ay] = 65535;
			}
	//		printf("%u %u \n", process_adc_value ->raw_value[adc_rank[ay]],process_adc_value->p_value[adc_rank[ay]]);
				
	}
			if(ay >= AY_NUM )
			{
				ay = 0;
			}

}


	

