/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#include	"config.h"
#include    "string.h"
#include	"STC8G_H_ADC.h"
#include	"STC8G_H_GPIO.h"
#include	"STC8G_H_Exti.h"
#include	"STC8G_H_UART.h"
#include	"STC8G_H_Delay.h"
#include	"STC8G_H_NVIC.h"
#include	"STC8G_H_Switch.h"
#include    "STC8G_H_EEPROM.h"

/*************	����˵��	**************

  �����̻���STC8H8K64UΪ����оƬ��ʵ����8���б�д���ԣ�STC8G��STC8Hϵ��оƬ��ͨ�òο�.

  ��ʾINT0~INT4 5������Դ��MCU�����߻���.

  �Ӵ����������Դ�����Ѵ�����115200,N,8,1.

  ����ʱ, ѡ��ʱ�� 22.1184MHz (�û�����"config.h"�޸�Ƶ��).

 ******************************************/

/*************	���س�������	**************/


/*************	���ر�������	**************/

u8 WakeUpCnt;

/*************	���غ�������	**************/



/*************  �ⲿ�����ͱ������� *****************/



/******************** IO������ ********************/
void GPIO_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;				//�ṹ����

    GPIO_InitStructure.Pin  = GPIO_Pin_3;			//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
    GPIO_InitStructure.Mode = GPIO_PullUp;			//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��

	/* adc 10 IO */
	GPIO_InitStructure.Pin =  GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_HighZ;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);

    /* uart1 IO */
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	
    /* LED O R */
	GPIO_InitStructure.Pin  = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);

    /* PLC STATE */
	GPIO_InitStructure.Pin  = GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);

}

/******************* AD���ú��� *******************/
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;		//�ṹ����

	ADC_InitStructure.ADC_SMPduty   = 31;		//ADC ģ���źŲ���ʱ�����, 0~31��ע�⣺ SMPDUTY һ����������С�� 10��
	ADC_InitStructure.ADC_CsSetup   = 0;		//ADC ͨ��ѡ��ʱ����� 0(Ĭ��),1
	ADC_InitStructure.ADC_CsHold    = 3;		//ADC ͨ��ѡ�񱣳�ʱ����� 0,1(Ĭ��),2,3
	ADC_InitStructure.ADC_Speed     = ADC_SPEED_2X16T;		//���� ADC ����ʱ��Ƶ��	ADC_SPEED_2X1T~ADC_SPEED_2X16T
	ADC_InitStructure.ADC_AdjResult = ADC_RIGHT_JUSTIFIED;	//ADC�������,	ADC_LEFT_JUSTIFIED,ADC_RIGHT_JUSTIFIED
	ADC_Inilize(&ADC_InitStructure);		//��ʼ��
	ADC_PowerControl(ENABLE);				//ADC��Դ����, ENABLE��DISABLE
	NVIC_ADC_Init(DISABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/******************** INT���� ********************/
void Exti_config(void)
{
    EXTI_InitTypeDef	Exti_InitStructure;							//�ṹ����

    Exti_InitStructure.EXTI_Mode      = EXT_MODE_Fall;//�ж�ģʽ,   EXT_MODE_RiseFall,EXT_MODE_Fall
    Ext_Inilize(EXT_INT1,&Exti_InitStructure);				//��ʼ��
    NVIC_INT1_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/****************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
    COMx_InitDefine		COMx_InitStructure;				//�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;		//ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;		//������, һ�� 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;			//��������,   ENABLE��DISABLE
    COMx_InitStructure.BaudRateDouble = DISABLE;		//�����ʼӱ�, ENABLE��DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);		//��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

//uint8 value_ladder[5] = {8, 33, 65, 90, 120};
uint8 value_ladder[5] = {8, 20, 35, 50, 65};

#define PRINTF_BUFFER_SIZE 32
#define FLASH_ADDRESS_START 0x00

/******************** ������***********************/
void main(void)
{
    uint8 flag = 0;
    uint8 printf_buffer[PRINTF_BUFFER_SIZE] = {0x00};
    uint8 printf_buffer_len = 0;
    int16 plc_flag = 0;
    uint16 adc_value = 0;
    uint32 time_10 = 0;
    uint16 addr = 0x04;

    GPIO_config();
    ADC_config();
    UART_config();
    //	Exti_config();
    EA  = 1;		//Enable all interrupt


    P54 = 0;

#if 0
	EEPROM_SectorErase(addr);           //��������
	EEPROM_write_n(addr,&RX1_Buffer[9],j);      //дN���ֽ�
	EEPROM_read_n(addr,tmp,j);
#endif

    PrintString1("system init finish\r\n");
    EEPROM_read_n(FLASH_ADDRESS_START, printf_buffer, 2);

    memcpy(&plc_flag, printf_buffer, 2);

    if (plc_flag < 3 || plc_flag > 1024)
        plc_flag = 3;


    sprintf(printf_buffer, "plc flag %d\r\n", plc_flag);
    PrintString1(printf_buffer);
    memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);


    while(1)
    {
        time_10 ++;
        delay_ms(20);	//delay 10ms

        Exti_config();

        if(WakeUpSource == 2)
        {
            PrintString1("���ж�INT1����  \r\n");
            EEPROM_SectorErase(FLASH_ADDRESS_START);
            memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);
            sprintf(printf_buffer, "write buffer %d\r\n", adc_value);
            PrintString1(printf_buffer);

            memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);
            memcpy(printf_buffer, &adc_value, 2);
            plc_flag = adc_value;
            
            EEPROM_write_n(FLASH_ADDRESS_START, printf_buffer, 2);
        }
        WakeUpSource = 0;

        adc_value = Get_ADCResult(10);
        if (adc_value > 1024) adc_value = 1024;

#if 0
        if (adc_value > value_ladder[0])
            P36 = 0;
        else P36 = 1;

        if (adc_value > value_ladder[1])
            P37 = 0;
        else P37 = 1;

        if (adc_value > value_ladder[2])
            P15 = 0;
        else P15 = 1;

        if (adc_value > value_ladder[3])
            P16 = 0;
        else P16 = 1;

        if (adc_value > value_ladder[4])
            P17 = 0;
        else P17 = 1;
#else
        if (adc_value >= value_ladder[0] && adc_value < value_ladder[1])
            P36 = 0;
        else P36 = 1;

        if (adc_value >= value_ladder[1] && adc_value < value_ladder[2])
            P37 = 0;
        else P37 = 1;

        if (adc_value >= value_ladder[2] && adc_value < value_ladder[3])
            P15 = 0;
        else P15 = 1;

        if (adc_value >= value_ladder[3] && adc_value < value_ladder[4])
            P16 = 0;
        else P16 = 1;

        if (adc_value >= value_ladder[4])
            P17 = 0;
        else P17 = 1;

#endif

        if (adc_value > plc_flag)
        {
            P54 = 0;
            P34 = 0;
        }
        else
        {
            P54 = 1;
            P34 = 1;
        }

        flag = ~flag;
     
        if (!(time_10 % 20))
        {
            //sprintf(printf_buffer, "adc value %d\r\n", adc_value);
            if (printf_buffer_len != 0)
            {
                //memset(printf_buffer, 0x00, PRINTF_BUFFER_SIZE);
                //EEPROM_read_n(FLASH_ADDRESS_START, printf_buffer, printf_buffer_len);
                //PrintString1(printf_buffer);
            }
        }
    }
}

