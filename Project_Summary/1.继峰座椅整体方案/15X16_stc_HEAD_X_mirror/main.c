/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include	"config.h"
#include	"STC32G_GPIO.h"
#include	"STC32G_ADC.h"
#include	"STC32G_UART.h"
#include	"STC32G_DMA.h"
#include	"STC32G_NVIC.h"
#include	"STC32G_Delay.h"
#include	"STC32G_Switch.h"
#include 	"stdio.h"
#include  "adc.h"
#include  "led.h"
#include "UART_DMA.h"

/*************   功能说明   ***************

本程序演示多路ADC DMA采样.

初始化时先把要ADC转换的引脚设置为高阻输入.

设置数据批量传输(DMA)功能，所有通道一次循环采集的数据自动存放到DMA定义的xdata空间.

通过串口2(P4.6 P4.7)将DMA定义的xdata空间所收到的数据发送给上位机，波特率115200,N,8,1

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度.

下载时, 选择时钟 22.1184MHz (可以在配置文件"config.h"中修改).

******************************************/

/*************	本地常量声明	**************/
#define ADC_VALUE_NUM ((AX_NUM - 1) * AY_NUM)
#define PACK_LEN (8 + ADC_VALUE_NUM)
#define sof 0x5aa5

/*************	本地变量声明	**************/

/*************	本地函数声明	**************/

void Uart1SendData(u8 *puts, u32 len);
u16 CalChecksum(u8 * put_data, u16 len);
/*************  外部函数和变量声明 *****************/
Adc_value adc_read;
/******************** UART配置 ********************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;		//结构定义

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//选择波特率发生器, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2, 所以不用选择)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE 或 DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口2 UART1,UART2,UART3,UART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	UART1_SW(UART1_SW_P30_P31);		//UART2_SW_P10_P11,UART2_SW_P46_P47
}


/******************** task A **************************/
void main(void)
{

	u8 serial_frame[PACK_LEN] = { 0 };
	u8 adc_raw_value[ADC_VALUE_NUM] = { 0 };
	u16 checksum = 0; 
	u8 count = 0;
	u32 time_1ms = 0;

	WTST = 0;		//设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
	EAXSFR();		//扩展SFR(XFR)访问使能 
	CKCON = 0;      //提高访问XRAM速度
	
  led_GPIO_config();

	UART_config();
	ADC_DMA_Init();

	
	EA = 1;
	
	
	while(1)
	{
		time_1ms ++;

		if(time_1ms % 500 == 0)
		{
			LED_GREEN = ~ LED_GREEN;		
		}
	
		
		if(time_1ms % 1 == 0)
		{
			AX_scan(count);
			
			Get_Voltage_Value(&adc_read, count);
	
			
			count ++;
			
					if(count == AX_NUM)
					{		
						count = 0;
						ProcessADCvalue(&adc_read);
					}
			
		}

		
		if(time_1ms % 13 == 0)
		{
			u8 ax = 0, ay = 0;
			u16 data_index = 0;
			
			serial_frame[0] = sof;
			serial_frame[1] = sof >> 8;
			
			serial_frame[2] = 0x01;
			
			serial_frame[3] = (PACK_LEN - 2) & 0x00ff;
			serial_frame[4] = (PACK_LEN - 2) >> 8;
			
			serial_frame[5] = 0x01;

			/*15X16*/
			for(ay = 0; ay < 16; ++ay)
			{
				for(ax = 1; ax < 16; ++ax)
				{

					serial_frame[6 + data_index] = adc_read.processed_adc_value[16 - ax][15 - ay];
					data_index ++;
				}
			}
			data_index = 0;
			
			checksum = CalChecksum(&serial_frame[0], PACK_LEN - 2);
			serial_frame[PACK_LEN - 2] = checksum & 0x00ff;
			serial_frame[PACK_LEN - 1] = checksum >> 8;
		
			Uart1SendData(serial_frame, PACK_LEN);
		}
		
		delay_ms(1);
	}
	
}


u16 CalChecksum(u8 * put_data, u16 len)
{
    u16 sum = 0;
	  u32 i;
    for(i = 0; i < len; ++i)
    {
        sum += put_data[i];
    }
    return sum;
}

void Uart1SendData(u8 *puts, u32 len)
{
		u32 i = 0;
		for(i = 0; i < len; ++i)
    {
	  	TX1_write2buff(puts[i]);
		}
}

