
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


/*����*/
#define SERIAL_DEBUG_OUTPUT 0
#define SERIAL_UI_OUTPUT 1
#define ADC_CHANNEL 4
#define SOF	0xaa555aa5
#define PACK_LEN (10 + ADC_CHANNEL * 2)	 //sof(4) + id(1) + len(2) + cmd(1) + value(ADC_CHANNEL * 2) + checksum(2)


/*************  ADC��ʼ������ *****************/
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//�ṹ����
	ADC_InitStructure.ADC_Px        = ADC_P11;	//����Ҫ��ADC��IO,	ADC_P10 ~ ADC_P17(�����),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_180T;			//ADC�ٶ�			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC��������/�ر�	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC�������,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//���ȼ�����	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//�ж�����		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//��ʼ��
	ADC_PowerControl(ENABLE);							//������ADC��Դ��������, ENABLE��DISABLE
}

void gpio_init()
{
	GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����	
	GPIO_InitStructure.Pin  = GPIO_Pin_0;		//ָ��Ҫ��ʼ����IO,
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//��ʼ��
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;		//ָ��Ҫ��ʼ����IO,
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//��ʼ��
	
	GPIO_InitStructure.Pin  = GPIO_Pin_3 | GPIO_Pin_2;		//ָ��Ҫ��ʼ����IO,
	GPIO_InitStructure.Mode = GPIO_OUT_OD;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��
	
	GPIO_InitStructure.Pin  = GPIO_Pin_6 | GPIO_Pin_7;		//ָ��Ҫ��ʼ����IO,
	GPIO_InitStructure.Mode = GPIO_OUT_OD;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��

}




/*************  ����1��ʼ������ *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//�ڲ���·RXD��TXD, ���м�, ENABLEDISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//��ʼ������1 USART1,USART2
}

/*************  У��ͼ��� *****************/
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
/**********************************************/
void main(void)
{
	u16 index = 0, key_value = 0, key_record = 0;
	GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����	
	u16 adc_value[ADC_CHANNEL] = { 0 };
	u16 temp_adc_value[ADC_CHANNEL] = 0;

	u8	sn74lv4051_index = 0;
	u32 timer_1ms = 0;

//	P_SW2 |= 0x80;
//  LED_RED = 0;
//	ADC_config();

//	UART_config();
	EA = 1;
//	P10 = ~P10;
	
	
    P3M0 = 0x00;
    P3M1 = 0x00;
 
	IO_VCC_CTRL_0 = 0;
	IO_VCC_CTRL_1 = 0;
	IO_ADC_CTRL_0 = 0;
	while(1);
	while (0)
	{
		time_1ms ++;
		if(time_1ms % 1 == 0)
		{
			LED_RED = ~LED_RED;
			BEEP_ENABLE = ~BEEP_ENABLE;
		}
		if(time_1ms % 1 == 0)
		{
			adc_value[1] = Get_ADC10bitResult(1);
			if (KEY_CLANNEL == 1)
			{
				adc_value[0] = 1000;
			} else
				adc_value[0] = 0;
			send_message_once(0 ,adc_value, 2);
			
			IO_VCC_CTRL_0 = 1;
			IO_VCC_CTRL_1 = 0;
			IO_VCC_CTRL_2 = 0;
			IO_ADC_CTRL_0 = 0;
			IO_ADC_CTRL_0 = 0;
			IO_ADC_CTRL_0 = 0;
		}
		if (0)	
		{
				key_value = adc_value[0];
			
				if (key_value > 1000 && key_record < 1000)
				{
						end_time_1ms = time_1ms + 5;
						key_record = key_value;
				}

				if (key_value < 1000 && key_record > 1000)
				{
						end_time_1ms = time_1ms + 5;
						key_record = key_value;
				}

				if (end_time_1ms == time_1ms && key_value > 1000)
				{
						send_message_once(0x01, 0, 1);
						GREEN_LED(0);
				}

				if (end_time_1ms == time_1ms && key_value < 1000)
				{

						send_message_once(0x02, 0, 1);
						new_test_time = time_1ms + 10;
						GREEN_LED(1);
				}

				if (new_test_time == time_1ms && key_value < 1000)
				{
						send_message_once(0x00, 0, 1);
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

		buff[4] = 0x01;
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



