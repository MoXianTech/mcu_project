#include	"config.h"
#include 	"STC8G_H_ADC.h"
#include	"STC8G_H_GPIO.h"
#include	"STC8G_H_NVIC.h"
#include	"STC8G_H_Switch.h"
#include 	"STC8G_H_Delay.h"
#include 	"STC8G_H_UART.h"
#include    "math_sensor.h"

/*引脚对应

ADC采样引脚
P3.4	ADC13	ADCA
P1.1	ADC1	ADCB
P3.3	ADC12	ADCC
P1.0	ADC0	ADCD

X扫描引脚
P1.4	AX1
P1.3	AX2
P1.2	AX3

LED引脚
P5.4	LED-W

串口引脚
P3.0	RX
P3.1	TX
*/

typedef char bool;

#define LOG_PRINTF 1

#if LOG_PRINTF

#else
typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[12];
    uint16_t checksum;
} SerialFrame_t;

#endif

typedef struct 
{
	uint16_t value[3][4];
	
#if LOG_PRINTF
#else
	uint16_t cali_value[3][4];
#endif
}StuAdcValue;

typedef enum {
	X1 = 1,
	X2,
	X3
}X_enum;

typedef enum{
	Y1 = 1,
	Y2,
	Y3,
	Y4,
}Y_enum;


#define AY1	ADC_CH12
#define AY2 ADC_CH1
#define AY3 ADC_CH0
#define AY4 ADC_CH11

sbit AX1 = P1^4;
sbit AX2 = P1^3;
sbit AX3 = P1^2;

sbit LED = P5^4;

uint32_t time_1ms = 0;
#if LOG_PRINTF
uint8_t printf_buffer[20] = {0};

#else
SerialFrame_t serial_frame;
#endif


void ReadAdcValue(StuAdcValue *adc_raw_value);
uint16_t CalChecksum(uint8_t * dat, uint16_t len);
uint16_t big_to_small(uint16_t dat);

/************************ IO口配置 ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;         //结构定义
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1;           //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_HighZ;          //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);      //初始化

	GPIO_InitStructure.Pin  = GPIO_Pin_3|GPIO_Pin_4;         //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_HighZ;          //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);      //初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1;         //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;          //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);      //初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;         //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;          //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);      //初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4;         //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_PullUp;          //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);      //初始化
}


/******************* AD配置函数 *******************/
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//结构定义

	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC 模拟信号采样时间控制, 0~31（注意： SMPDUTY 一定不能设置小于 10）
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC 通道选择时间控制 0(默认),1
	ADC_InitStructure.ADC_CsHold    = 1;		//ADC 通道选择保持时间控制 0,1(默认),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X16T;		//设置 ADC 工作时钟频率	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC结果调整,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_Inilize(&ADC_InitStructure);		//初始化
	ADC_PowerControl(ENABLE);				//ADC电源开关, ENABLE或DISABLE
	NVIC_ADC_Init(DISABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}


/***************  串口初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2, 所以不用选择)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口2 USART1,USART2,USART3,USART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
	UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}


/************************** 主函数**********************/
void main(void)
{
	static bool led_flag = FALSE;
    StuAdcValue temp_adc_value;
    int32 adc_value = 0, total_value;


	uint8_t i = 0;
	
	EAXSFR();
	GPIO_config();
	UART_config();
	ADC_config();
	EA = 1;
	
	while(1)
	{
		time_1ms ++;
        //LED闪烁
		if(time_1ms % 5 == 0)
		{
			LED = 1;
		}
		
		if(time_1ms % 2000 == 0)
		{
			led_flag = !led_flag;
			LED = 0;
		}
		

		if(time_1ms % 14 == 0)
		{
			ReadAdcValue(&temp_adc_value);
			
		}
		if(time_1ms % 14 == 0)
		{
 //数据排列转换
#if LOG_PRINTF
            //adc_calculation_calibration_once((void *)temp_adc_value.value, (void *)temp_adc_value.cali_value, 1, printf_buffer);
			PrintString1(printf_buffer);
			sprintf(printf_buffer, "adc_value = %u",temp_adc_value.value[0][0]);
			PrintString1(printf_buffer);
			sprintf(printf_buffer, " %u",temp_adc_value.value[1][0]);
			PrintString1(printf_buffer);
			sprintf(printf_buffer, " %u  ",temp_adc_value.value[2][0]);
			PrintString1(printf_buffer);

            total_value = temp_adc_value.value[1][0] + temp_adc_value.value[2][0] + temp_adc_value.value[0][0];
			sprintf(printf_buffer, "total %ld  ", adc_value);

			PrintString1(printf_buffer);
	
			
			            adc_value =  (1024 - total_value) * 1000 / (uint32_t)temp_adc_value.value[0][0];
            sprintf(printf_buffer, "cal = %lld", adc_value);
			PrintString1(printf_buffer);
			
						            adc_value =  (1024 - total_value) * 1000 / (uint32_t)temp_adc_value.value[1][0];
            sprintf(printf_buffer, " %lld", adc_value);
			PrintString1(printf_buffer);
			
						            adc_value =  (1024 - total_value) * 1000 / (uint32_t)temp_adc_value.value[2][0];
            sprintf(printf_buffer, " %lld\r\n", adc_value);
			PrintString1(printf_buffer);
			
			
//			
//            adc_value = 1024 * (uint32_t)temp_adc_value.value[0][0] / (1024 - (total_value - temp_adc_value.value[0][0]));
//            sprintf(printf_buffer, "cal = %lld\r", adc_value);
//			PrintString1(printf_buffer);
//			
//			            adc_value = 1024 * (uint32_t)temp_adc_value.value[1][0] / (1024 - (total_value - temp_adc_value.value[1][0]));
//            sprintf(printf_buffer, " %lld\r", adc_value);
//			PrintString1(printf_buffer);
//			
//			            adc_value = 1024 * (uint32_t)temp_adc_value.value[2][0] / (1024 - (total_value - temp_adc_value.value[2][0]));
//            sprintf(printf_buffer, " %lld\r\n", adc_value);
//			PrintString1(printf_buffer);
#else			
			serial_frame.sof = 0xa55a;
			serial_frame.tran_type = 0x01;
			serial_frame.len = big_to_small(sizeof(serial_frame) - 2);
			serial_frame.type = 0x01;

      adc_calculation_calibration_once((void *)temp_adc_value.value, (void *)temp_adc_value.cali_value, 0);
			
#if 0            
			serial_frame.adc_value[0] = temp_adc_value.value[1][0]/4;
			serial_frame.adc_value[1] = temp_adc_value.value[1][3]/4;
			serial_frame.adc_value[2] = temp_adc_value.value[1][1]/4;
			serial_frame.adc_value[3] = temp_adc_value.value[1][2]/4;
			
			serial_frame.adc_value[4] = temp_adc_value.value[0][0]/4;
			serial_frame.adc_value[5] = temp_adc_value.value[0][3]/4;
			serial_frame.adc_value[6] = temp_adc_value.value[0][1]/4;
			serial_frame.adc_value[7] = temp_adc_value.value[0][2]/4;
			
			serial_frame.adc_value[8] = temp_adc_value.value[2][0]/4;
			serial_frame.adc_value[9] = temp_adc_value.value[2][3]/4;
			serial_frame.adc_value[10] = temp_adc_value.value[2][1]/4;
			serial_frame.adc_value[11] = temp_adc_value.value[2][2]/4;

#else
			serial_frame.adc_value[0] = temp_adc_value.cali_value[1][0];
			serial_frame.adc_value[1] = temp_adc_value.cali_value[1][3];
			serial_frame.adc_value[2] = temp_adc_value.cali_value[1][1];
			serial_frame.adc_value[3] = temp_adc_value.cali_value[1][2];
			
			serial_frame.adc_value[4] = temp_adc_value.cali_value[0][0];
			serial_frame.adc_value[5] = temp_adc_value.cali_value[0][3];
			serial_frame.adc_value[6] = temp_adc_value.cali_value[0][1];
			serial_frame.adc_value[7] = temp_adc_value.cali_value[0][2];
			
			serial_frame.adc_value[8] = temp_adc_value.cali_value[2][0];
			serial_frame.adc_value[9] = temp_adc_value.cali_value[2][3];
			serial_frame.adc_value[10] = temp_adc_value.cali_value[2][1];
			serial_frame.adc_value[11] = temp_adc_value.cali_value[2][2];

#endif
			
			serial_frame.checksum = CalChecksum((uint8_t *)&serial_frame, sizeof(serial_frame) - 2);
			serial_frame.checksum = big_to_small(serial_frame.checksum);
			
			send_data((uint8_t *)&serial_frame, sizeof(serial_frame));
#endif
		}
		
		
		delay_ms(1);
	}
}

//大小端互换
uint16_t big_to_small(uint16_t dat)
{
	char right, left;
	uint16_t temp;
	right = dat & 0XFF;//低八位
	left = dat >> 8;//高八位  右移8位
	temp = right * 256 + left;
	
	return temp;
}

uint16_t CalChecksum(uint8_t * dat, uint16_t len)
{
    uint16_t sum = 0;
		uint8_t i = 0;
    for(i = 0; i < len; ++i)
    {
        sum += dat[i];
    }
    return sum;
}

/*
功能：选择行导通
输入：第几行
*/
void ChooseChanneX(uint8_t dat)
{
	X_enum x_sort[3] = {X1,X2,X3};
	
	#if 1
	AX1 = (X1==x_sort[dat-1]) ? 0 : 1;
  AX2 = (X2==x_sort[dat-1]) ? 0 : 1;
	AX3 = (X3==x_sort[dat-1]) ? 0 : 1; 

	#else
	AX1 = (X1==x_sort[dat-1]) ? 1 : 0;
  AX2 = (X2==x_sort[dat-1]) ? 1 : 0;
	AX3 = (X3==x_sort[dat-1]) ? 1 : 0; 
	#endif
}

/*
功能：选择列导通
输入：第几行
*/
uint8_t ChooseChanneY(uint8_t dat)
{
	uint8_t AY = 0;
	Y_enum y_sort[4] = {Y1,Y2,Y3,Y4};
	
	switch(y_sort[dat-1])
	{
		case Y1: AY = AY1;
			break;
		case Y2: AY = AY2;
			break;
		case Y3: AY = AY3;
			break;
		case Y4: AY = AY4;
			break;
	}
	
	return AY;
}


/*
功能：获取均值滤波后的数据
输入：ADC采样通道，滤波次数
返回：返回ADC值
*/
uint16_t GetAdcValue(uint8_t index,uint8_t count)
{
	uint8_t i = 0;
	uint16_t sum = 0;
	 
	
	for(i = 0; i < count; ++i)
	{
		sum += Get_ADCResult(index);
		
	}
	
	return sum/count;
}


/*
功能：行列扫描读取数据
输入：无
返回：返回3*4的数据
*/
void ReadAdcValue(StuAdcValue *adc_raw_value)
{
	uint8_t x = 0;
	uint8_t y = 0;
	
	for(x = 0; x <	3; ++x)
		{
			ChooseChanneX(x+1);
			for(y = 0; y < 4; ++y)
			{
				adc_raw_value->value[x][y] = GetAdcValue(ChooseChanneY(y+1), 5);
			}
		}
}


