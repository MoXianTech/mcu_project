
#include	"config.h"
#include	"adc.h"
#include	"delay.h"
#include	"USART1.h"
#include	<string.h>
#include    "SN74LV4051.h"
#include	"STC8G_H_GPIO.h"
#include    <stdio.h>
#include    <stdarg.h>
#include    "data_input.h"

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
#define ADC_CHANNEL 8
#define SOF	0xaa555aa5
#define PACK_LEN (10 + ADC_CHANNEL * 2)	 //sof(4) + id(1) + len(2) + cmd(1) + value(ADC_CHANNEL * 2) + checksum(2)

#define R_DEF_VALUE 10000
#define R_SEND_MULT 10
#define SENSOR_PIN_TOTAL 4
#define SENSOR_NUMBER 2

typedef enum {
    PERI_LED,
    PERI_BEEP
} PERIPHERAL_TYPE_NUM_T;

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

void peripheral_reversal_number(u8 reversal_number,
        u16 interval_time,
        PERIPHERAL_TYPE_NUM_T peri_type)
{
    while(reversal_number --)
    {
        switch(peri_type)
        {
            case PERI_LED:
                LED_RED = 0;
                delay_ms(interval_time);
                LED_RED = 1;
                break;

            case PERI_BEEP:
                BEEP_ENABLE = 0;
                delay_ms(interval_time);
                BEEP_ENABLE = 1;
                break;

            default:
                break;
        }
        delay_ms(100);
    }
}

typedef struct {
    u16 trigger_end_value;
    u16 trigger_start_value;
    u16 target_start_sum_limit_max;
    u16 target_start_sum_limit_min;
    u16 target_end_sum_limit_max;
    u16 target_end_sum_limit_min;
    u16 target_start_sum;
    u16 target_end_sum;
} param_value_t;

#define MAX_ADC_VALUE 1024
#define COUNT_START_VALUE_TRIGGER 0.2
#define COUNT_END_VALUE_TRIGGER 0.08

#define TARGET_START_VALUE_LIMIT 0.2
#define TARGET_END_VALUE_LIMIT_MAX 0.5
#define TARGET_END_VALUE_LIMIT_MIN 0.4

#define ADC_RECORD_END_COUNT_NUM 10
#define ADC_RECORD_START_COUNT_NUM 2

u16 get_buffer_sum_value(u16 *buffer, u8 buffer_size)
{
    u16 sum_value = 0x00;

    sum_value = buffer[0];

    while(-- buffer_size)
        sum_value += buffer[buffer_size];

    return sum_value;
}

void update_start_param_limit(param_value_t *param_value, u16 target_value)
{
    param_value->target_start_sum_limit_max = target_value * (1 + TARGET_START_VALUE_LIMIT);
    param_value->target_start_sum_limit_min = target_value * (1 - TARGET_START_VALUE_LIMIT);
}

void update_end_param_limit(param_value_t *param_value, u16 target_value)
{
    param_value->target_end_sum_limit_max = target_value * (1 + TARGET_END_VALUE_LIMIT_MAX);
    param_value->target_end_sum_limit_min = target_value * (1 - TARGET_END_VALUE_LIMIT_MIN);
    param_value->trigger_end_value = param_value->target_end_sum_limit_max / ADC_RECORD_END_COUNT_NUM;
}

void math_param_init(param_value_t *param_value, u16 default_value)
{
    param_value->trigger_start_value = default_value + MAX_ADC_VALUE * COUNT_START_VALUE_TRIGGER;
    param_value->trigger_end_value = default_value + MAX_ADC_VALUE * COUNT_END_VALUE_TRIGGER;
    param_value->target_end_sum_limit_max = param_value->trigger_end_value * ADC_RECORD_END_COUNT_NUM * (1 + TARGET_START_VALUE_LIMIT);
    param_value->target_end_sum_limit_min = param_value->trigger_end_value * ADC_RECORD_END_COUNT_NUM * (1 - TARGET_START_VALUE_LIMIT);
}

typedef enum {
    START_TRIGGER,
    END_TRIGGER
} TRIGGER_TYPE_T;

u8 check_value_limit_range(param_value_t *param_value, u16 *record_buffer, u8 buffer_size, TRIGGER_TYPE_T trigger_type)
{
    u16 cache_sum = get_buffer_sum_value(record_buffer, buffer_size);
    u8 rtv = 0;

    switch (trigger_type)
    {
        case START_TRIGGER:
            break;
        case END_TRIGGER:
            if (cache_sum < param_value->target_end_sum_limit_max)
                rtv = 1;
            else
                rtv = 0;
            if (cache_sum < param_value->target_end_sum_limit_min)
            {
                update_end_param_limit(param_value, cache_sum);
                rtv = 2;
            }
            break;
        default:
            break;
    }

    return rtv;
}

//#define READ_TEST_DATA

/**********************************************/
void main(void)
{
    u32 timer_1ms = 0, count_number_sum = 0;
    u16 adc_value[ADC_CHANNEL] = { 0 };
    u16 default_value = 0;
    param_value_t param_value = {0};

    u16 target_start_value[ADC_RECORD_START_COUNT_NUM] = {0};
    u16 target_end_value[ADC_RECORD_START_COUNT_NUM] = {0};

    u16 record_start_value[ADC_RECORD_START_COUNT_NUM] = {0};
    u16 record_end_value[ADC_RECORD_END_COUNT_NUM] = {0};

    u16 target_end_sum_value = 0, target_start_sum_value = 0;
    u16 record_end_sum_value = 0, record_start_sum_value = 0;

    u8 count_start_record = 0, count_end_record = 0, count = 0;
    u8 count_flag = 0;
    u8 rtl = 0;

#ifdef READ_TEST_DATA
    //测试数据导入，正常不需要
    u16 count_data_input = 0;
    u16 count_data_input_lens = sizeof(data_input) / 2;
#endif

    ADC_config();

    UART_config();
    EA = 1;
    LED_RED = 1;

    delay_ms(300);
    //上电获取adc值
    default_value = get_once_adc_value(1, 2);

    //设置adc阈值
#ifdef  READ_TEST_DATA
    default_value = data_input[0];
#endif

    math_param_init(&param_value, default_value);

#ifdef READ_TEST_DATA
    peripheral_reversal_number(5, 30, PERI_BEEP);
#else
    peripheral_reversal_number(3, 30, PERI_BEEP);
#endif
    while (1)
    {
        time_1ms ++;
        if(time_1ms % 1 == 0)
        {
            LED_RED = ~LED_RED;
            BEEP_ENABLE = 1;
        }
        if(time_1ms % 1 == 0)
        {
            //获取数据
            {
                //获取adc的值
                adc_value[0] = get_once_adc_value(1, 2);

#ifdef  READ_TEST_DATA
                //读取样本数据用于测试算法，产品直接获取adc即可
                if (count_data_input < count_data_input_lens)
                {
                    adc_value[0] = data_input[count_data_input ++];
                }
#endif
            }

            //处理数据
            {
                //把adc的值往上抬一些方便计算
                adc_value[0] += 15;


                //记录数值
                record_start_value[count_start_record ++] = adc_value[0];
                if (count_start_record >= ADC_RECORD_START_COUNT_NUM) count_start_record = 0;

                record_end_value[count_end_record ++] = adc_value[0];
                if (count_end_record >= ADC_RECORD_END_COUNT_NUM) count_end_record = 0;

                //检测跪下一段时间的数值是否稳定
                count_flag |= 0x01;
                for(count = 0; count < ADC_RECORD_START_COUNT_NUM; count ++)
                {
                    if (record_start_value[count] < param_value.trigger_start_value)
                    {
                        count_flag &= (~0x01);
                        break;
                    }
                }

                if ((count_flag & 0x01) == 0x01)
                {
                    count_flag |= 0x04;
                }

                count_flag |= 0x02;
                //检测站起来后数值是否是稳定的
                for(count = 0; count < ADC_RECORD_END_COUNT_NUM; count ++)
                {
                    if (record_end_value[count] > param_value.trigger_end_value)
                    {
                        count_flag &= (~0x02);
                        break;
                    }
                }

                //debug数据打印
                if ((count_flag & 0x06) == 0x06)
                    adc_value[4] = get_buffer_sum_value(record_end_value, ADC_RECORD_END_COUNT_NUM);

                adc_value[1] = param_value.trigger_end_value;
                adc_value[2] = param_value.target_end_sum_limit_min;
                adc_value[3] = param_value.target_end_sum_limit_max;


                //判断是否有跪下+起来稳定的标志
                if ((count_flag & 0x06) == 0x06)
                {
                    //如果起来后稳定的数据离原始差太多，则更新稳定的阈值
                    rtl = check_value_limit_range(&param_value, record_end_value, ADC_RECORD_END_COUNT_NUM, END_TRIGGER); 

                    switch (rtl)
                    {
                        case 0:
                            //不计数
                            break;
                        case 1:
                            //计数一次
                            peripheral_reversal_number(2, 50, PERI_BEEP);
                            break;
                        case 2:
                            //第一次学习会计数一次，后面学习不计数
                            if (count_number_sum < 1)
                                peripheral_reversal_number(2, 50, PERI_BEEP);
                            if (count_number_sum > 2)
                                peripheral_reversal_number(2, 50, PERI_BEEP);
                            break;
                    }
                    count_flag = 0x00;
                    count_number_sum ++;
                }

                adc_value[5] = rtl;
                send_message_once(1, adc_value, ADC_CHANNEL);
            }
        }

#ifdef READ_TEST_DATA
        //读取数据测试时加快读取速度
        delay_ms(20);
#else
        delay_ms(100);
#endif
    }
}

u16 get_once_adc_value(u8 io_channel, u8 adc_channel)
{
    u16 adc_value_temp = 0;
    set_vcc_channel(io_channel);
    set_adc_channel(adc_channel);
    delay_ms(5);
    adc_value_temp = Get_ADC10bitResult(1);
    if (adc_value_temp < 10)
        return 0;
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
