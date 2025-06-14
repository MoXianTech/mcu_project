
#include	"config.h"
#include	"adc.h"
#include	"delay.h"
#include	"USART1.h"
#include	"string.h"
#include    "SN74LV4051.h"
#include	"STC8G_H_GPIO.h"

/*led*/
sbit LED_RED = P3^3;

sbit IO_VCC_CTRL_0 = P1^0;
sbit IO_VCC_CTRL_1 = P1^2;
sbit IO_VCC_CTRL_2 = P1^3;
sbit IO_VCC_CTRL_3 = P1^4;

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
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;		//指定要初始化的IO,
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//初始化
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

#define DURATION_TIME 18000
#define DURATION_TIME_ON 4000
/**********************************************/
void main(void)
{
	u16 index = 0, key_value = 0, key_record = 0;
	u16 adc_value[ADC_CHANNEL] = { 0 };
	u16 r_value[ADC_CHANNEL] = { 0 };
	u16 temp_adc_value[ADC_CHANNEL] = 0;


	u8	sn74lv4051_index = 0;
	u32 timer_1ms = 0;
    u32 duration_count = 0;

    LED_RED = 0;

	UART_config();
	EA = 1;

	LED_RED = 1;
    gpio_init();

	while (1)
	{
		time_1ms ++;
        duration_count = time_1ms % DURATION_TIME;

		if(duration_count > DURATION_TIME_ON)
		{
            IO_VCC_CTRL_0 = 1;
            IO_VCC_CTRL_1 = 1;
            IO_VCC_CTRL_2 = 1;
            IO_VCC_CTRL_3 = 1;
		} else {
            IO_VCC_CTRL_0 = 0;
            IO_VCC_CTRL_1 = 0;
            IO_VCC_CTRL_2 = 0;
            IO_VCC_CTRL_3 = 0;
        }

		delay_ms(100);
	}
}

