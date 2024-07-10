
#include	"config.h"
#include	"adc.h"
#include	"delay.h"
#include	"USART1.h"
#include	"string.h"
#include    "SN74LV4051.h"
#include	"STC8G_H_GPIO.h"

/*led*/
sbit LED_RED = P3^3;
sbit BEEP_ENABLE = P3^2;
sbit KEY_CLANNEL = P1^0;
sbit IO_VCC_CTRL_0 = P3^7;
sbit IO_VCC_CTRL_1 = P3^6;
sbit IO_VCC_CTRL_2 = P1^2;

sbit IO_ADC_CTRL_0 = P1^5;
sbit IO_ADC_CTRL_1 = P1^4;
sbit IO_ADC_CTRL_2 = P1^3;


/*串口*/
#define SERIAL_DEBUG_OUTPUT 0
#define SERIAL_UI_OUTPUT 1
#define ADC_CHANNEL 6
#define SOF	0xaa555aa5
#define PACK_LEN (10 + ADC_CHANNEL * 2)	 //sof(4) + id(1) + len(2) + cmd(1) + value(ADC_CHANNEL * 2) + checksum(2)

#define R_DEF_VALUE 10000
#define R_SEND_MULT 10
#define SENSOR_PIN_TOTAL 4
#define SENSOR_NUMBER 2

/*************  ADC初始化函数 *****************/
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//结构定义
	ADC_InitStructure.ADC_Px        = ADC_P11;	//设置要做ADC的IO,	ADC_P10 ~ ADC_P17(或操作),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_180T;			//ADC速度			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC功率允许/关闭	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC结果调整,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//优先级设置	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//中断允许		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//初始化
	ADC_PowerControl(ENABLE);							//单独的ADC电源操作函数, ENABLE或DISABLE
}

void gpio_init()
{
	GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义	
	GPIO_InitStructure.Pin  = GPIO_Pin_0;		//指定要初始化的IO,
	GPIO_InitStructure.Mode = GPIO_HighZ;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;		//指定要初始化的IO,
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_3 | GPIO_Pin_2;		//指定要初始化的IO,
	GPIO_InitStructure.Mode = GPIO_OUT_OD;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_6 | GPIO_Pin_7;		//指定要初始化的IO,
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//初始化

}




/*************  串口1初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLEDISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2
}

/*************  校验和计算 *****************/
unsigned short CalCheckSum(unsigned char * msg, unsigned char len)
{
	unsigned short sum = 0, i = 0;
	for(i = 0; i < len; ++i)
	{
		sum += msg[i];	
	}	
	return sum;
}

#define GREEN_LED(a)	if (a)\
				LED_RED = 1;\
				else		\
				LED_RED = 0
				
#define ADC_CHANNEL_STD_0(a)	if (a)\
				ADC_CLANNEL_0 = 1;\
				else		\
				ADC_CLANNEL_0 = 0				
				
void send_message_once(u8 command, u16 *adc_value, u8 adc_num);
u32 time_1ms = 0, end_time_1ms = 0, new_test_time = 0;
void set_adc_channel(u8 enable_channel);
void set_vcc_channel(u8 enable_channel);
u16 get_once_adc_value(u8 io_channel, u8 adc_channel);
void adc2R(u16 *adc_value, u8 adc_channel, u16 *r_value, u16 r_def_value);
/**********************************************/
void main(void)
{
	u16 index = 0, key_value = 0, key_record = 0;
	u16 adc_value[ADC_CHANNEL] = { 0 };
	u16 r_value[ADC_CHANNEL] = { 0 };
	u16 temp_adc_value[ADC_CHANNEL] = 0;

	u8	sn74lv4051_index = 0;
	u32 timer_1ms = 0;

  LED_RED = 0;
	ADC_config();

	UART_config();
	EA = 1;

	LED_RED = 1;

	while (1)
	{
		time_1ms ++;
		if(time_1ms % 1 == 0)
		{
//			LED_RED = ~LED_RED;
			BEEP_ENABLE = 1;
		}
		if(time_1ms % 1 == 0)
		{
//			adc_value[1] = Get_ADC10bitResult(1);
			if (KEY_CLANNEL == 1)
			{
				adc_value[1] = 0;
			} else
				adc_value[1] = 1100;
//			send_message_once(0 ,adc_value, 2);
		}
		if (1)	
		{
				key_value = adc_value[1];
			
				if (key_value > 1000 && key_record < 1000)
				{
						end_time_1ms = time_1ms + 10;
						key_record = key_value;
				}

				if (key_value < 1000 && key_record > 1000)
				{
						end_time_1ms = time_1ms + 2;
						key_record = key_value;
				}

				if (end_time_1ms == time_1ms && key_value > 1000)
				{
						adc_value[0] = get_once_adc_value(3, 1);
						adc_value[1] = get_once_adc_value(3, 2);
						adc_value[2] = get_once_adc_value(3, 4);
					
						adc_value[3] = get_once_adc_value(3 + SENSOR_PIN_TOTAL, 1 + SENSOR_PIN_TOTAL);
						adc_value[4] = get_once_adc_value(3 + SENSOR_PIN_TOTAL, 2 + SENSOR_PIN_TOTAL);
						adc_value[5] = get_once_adc_value(3 + SENSOR_PIN_TOTAL, 4 + SENSOR_PIN_TOTAL);
						adc2R(adc_value, SENSOR_PIN_TOTAL * SENSOR_NUMBER, r_value, R_DEF_VALUE / R_SEND_MULT);
						send_message_once(0x01, r_value, SENSOR_PIN_TOTAL * SENSOR_NUMBER);
					
						delay_ms(500);
						adc_value[0] = get_once_adc_value(1, 2);
						adc_value[1] = get_once_adc_value(1, 4);
						adc_value[2] = get_once_adc_value(2, 4);
						adc_value[3] = get_once_adc_value(1 + SENSOR_PIN_TOTAL, 2 + SENSOR_PIN_TOTAL);
						adc_value[4] = get_once_adc_value(1 + SENSOR_PIN_TOTAL, 4 + SENSOR_PIN_TOTAL);
						adc_value[5] = get_once_adc_value(2 + SENSOR_PIN_TOTAL, 4 + SENSOR_PIN_TOTAL);
						adc2R(adc_value, SENSOR_PIN_TOTAL * SENSOR_NUMBER, r_value, R_DEF_VALUE / R_SEND_MULT);
						send_message_once(0x04, r_value, SENSOR_PIN_TOTAL * SENSOR_NUMBER);
					
						GREEN_LED(0);
				}

				if (end_time_1ms == time_1ms && key_value < 1000)
				{
						adc_value[0] = get_once_adc_value(1, 2);
						adc2R(adc_value, 1, r_value, R_DEF_VALUE / R_SEND_MULT);
//						send_message_once(0x02, r_value, 1);
						new_test_time = time_1ms + 1;
						GREEN_LED(1);
				}

				if (new_test_time == time_1ms && key_value < 1000)
				{
						adc_value[0] = get_once_adc_value(3, 1);
						adc_value[1] = get_once_adc_value(3, 2);
						adc_value[2] = get_once_adc_value(3, 4);
						adc2R(adc_value, 3, r_value, R_DEF_VALUE / R_SEND_MULT);
//						send_message_once(0x00, r_value, 3);
						GREEN_LED(0);
						delay_ms(50);
						GREEN_LED(1);
						delay_ms(50);
						GREEN_LED(0);
						delay_ms(50);
						GREEN_LED(1);
						delay_ms(50);
						GREEN_LED(0);
						delay_ms(50);
						GREEN_LED(1);
				}
		}

		delay_ms(100);
	}
}


void adc2R(u16 *adc_value, u8 adc_channel, u16 *r_value, u16 r_def_value)
{
	u8 count = 0;
	for (count = 0; count < adc_channel; count ++)
	{
		r_value[count] = (1150.0 - adc_value[count]) / adc_value[count] * r_def_value;
	}
}

u16 get_once_adc_value(u8 io_channel, u8 adc_channel)
{
	u16 adc_value_temp = 0;
	set_vcc_channel(io_channel);
	set_adc_channel(adc_channel);
	delay_ms(100);
	adc_value_temp = Get_ADC10bitResult(1);
	if (adc_value_temp < 18)
		return 18;
	else
		return adc_value_temp;
}

u8 channel_range[8] = {5, 7, 6, 4, 3, 0, 1, 2};
void set_vcc_channel(u8 enable_channel)
{
		enable_channel --;
		IO_VCC_CTRL_0 = channel_range[enable_channel] & 0x01;
		IO_VCC_CTRL_1 = channel_range[enable_channel] & 0x02;
		IO_VCC_CTRL_2 = channel_range[enable_channel] & 0x04;
}

void set_adc_channel(u8 enable_channel)
{
		enable_channel --;
		IO_ADC_CTRL_0 = channel_range[enable_channel] & 0x01;
		IO_ADC_CTRL_1 = channel_range[enable_channel] & 0x02;
		IO_ADC_CTRL_2 = channel_range[enable_channel] & 0x04;
}

void send_message_once(u8 command, u16 *adc_value, u8 adc_num)
{
		u8 buff[PACK_LEN] = { 0 };
		u16 checksum = 0;
		u8 i = 0;
		u8 ref_command = command;
		buff[0] = SOF;
		buff[1] = SOF >> 8;
		buff[2] = SOF >> 16;
		buff[3] = SOF >> 24;

		buff[4] = ref_command;
		buff[5] = (PACK_LEN - 2) & 0x00ff;
		buff[6] = (PACK_LEN - 2) >> 8;
		buff[7] = 0x01;
		
		for(i = 0; i < adc_num; i++)
		{
			buff[8 + i * 2] = adc_value[i] & 0x00ff;
			buff[9 + i * 2] = adc_value[i] >> 8;
		}
		//memcpy(&buff[8], adc_value, ADC_CHANNEL * 2);
		checksum = CalCheckSum(&buff[0], PACK_LEN - 2);
		buff[PACK_LEN - 2] = checksum & 0x00ff;
		buff[PACK_LEN - 1] = checksum >> 8;
		
		Uart1SendData(buff, PACK_LEN);
}



