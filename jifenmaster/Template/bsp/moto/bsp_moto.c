#include "bsp_moto.h"
#include "board.h"


/******************************************************************
 * �� �� �� �ƣ�MOTO_Init
 * �� �� ˵ ����PWM����
 * �� �� �� �Σ� pre��ʱ��ʱ��Ԥ��Ƶֵ    per����
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��PWMƵ��=168 000 000 /( (pre+1) * (per+1) ) 
 ******************************************************************/


/* moto flag dire  0000, 0,0,0,0 = 4, 3, 2, 1*/
volatile uint8_t moto_dire_flag = 0x00;

void moto_init(uint16_t pre,uint16_t per)
{                             
    timer_parameter_struct timere_initpara={0};                 // ���嶨ʱ���ṹ��
    timer_oc_parameter_struct timer_ocintpara={0};              //��ʱ���Ƚ�����ṹ��

    //��ʱ��ʱ��
    rcu_periph_clock_enable(RCU_IN1_TIMER);     // ������ʱ��ʱ��
    rcu_periph_clock_enable(RCU_IN2_TIMER);                        
    rcu_periph_clock_enable(RCU_IN3_TIMER);                        
    rcu_periph_clock_enable(RCU_IN4_TIMER);
    rcu_periph_clock_enable(RCU_IN5_TIMER);     
    rcu_periph_clock_enable(RCU_IN6_TIMER);                        
    rcu_periph_clock_enable(RCU_IN7_TIMER);                        
    rcu_periph_clock_enable(RCU_IN8_TIMER);
    //����ʱ��
    rcu_periph_clock_enable(RCU_IN1);
    rcu_periph_clock_enable(RCU_IN2);
    rcu_periph_clock_enable(RCU_IN3);
    rcu_periph_clock_enable(RCU_IN4);
    rcu_periph_clock_enable(RCU_IN5);
    rcu_periph_clock_enable(RCU_IN6);
    rcu_periph_clock_enable(RCU_IN7);
    rcu_periph_clock_enable(RCU_IN8);

    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);  // ���ö�ʱ��ʱ��

    /* ����AIN1*/
    gpio_mode_set(PORT_IN1,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN1);
    gpio_output_options_set(PORT_IN1,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN1);
    gpio_af_set(PORT_IN1,AF_IN1,GPIO_IN1);        
    /* ����AIN2*/
    gpio_mode_set(PORT_IN2,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN2);
    gpio_output_options_set(PORT_IN2,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN2);
    gpio_af_set(PORT_IN2,AF_IN2,GPIO_IN2);            

    /* ����BIN1*/
    gpio_mode_set(PORT_IN3,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN3);
    gpio_output_options_set(PORT_IN3,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN3);
    gpio_af_set(PORT_IN3,AF_IN3,GPIO_IN3);        
    /* ����BIN2*/
    gpio_mode_set(PORT_IN4,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN4);
    gpio_output_options_set(PORT_IN4,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN4);
    gpio_af_set(PORT_IN4,AF_IN4,GPIO_IN4);
    /* ����CIN1*/
    gpio_mode_set(PORT_IN5,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN5);
    gpio_output_options_set(PORT_IN5,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN5);
    gpio_af_set(PORT_IN5,AF_IN5,GPIO_IN5);        
    /* ����CIN2*/
    gpio_mode_set(PORT_IN6,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN6);
    gpio_output_options_set(PORT_IN6,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN6);
    gpio_af_set(PORT_IN6,AF_IN6,GPIO_IN6);            

    /* ����DIN1*/
    gpio_mode_set(PORT_IN7,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN7);
    gpio_output_options_set(PORT_IN7,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN7);
    gpio_af_set(PORT_IN7,AF_IN7,GPIO_IN7);        
    /* ����DIN2*/
    gpio_mode_set(PORT_IN8,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN8);
    gpio_output_options_set(PORT_IN8,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN8);
    gpio_af_set(PORT_IN8,AF_IN8,GPIO_IN8);        


    /* ���ö�ʱ������ */
    timer_deinit(BSP_IN1_TIMER);        
    timer_deinit(BSP_IN2_TIMER);
    timer_deinit(BSP_IN3_TIMER);
    timer_deinit(BSP_IN4_TIMER);
    timer_deinit(BSP_IN5_TIMER);        
    timer_deinit(BSP_IN6_TIMER);
    timer_deinit(BSP_IN7_TIMER);
    timer_deinit(BSP_IN8_TIMER);

    // ��λ��ʱ��
    timere_initpara.prescaler = pre-1;      // ʱ��Ԥ��Ƶֵ     
    timere_initpara.alignedmode = TIMER_COUNTER_EDGE;              // ��Ե����                
    timere_initpara.counterdirection = TIMER_COUNTER_UP;    // ���ϼ���                                
    timere_initpara.period = per-1;                                              // ����     
    timere_initpara.clockdivision = TIMER_CKDIV_DIV1;            // ��Ƶ����
    timere_initpara.repetitioncounter = 0;                  // �ظ������� 0-255

    timer_init(BSP_IN1_TIMER,&timere_initpara);        // ��ʼ����ʱ��
    timer_init(BSP_IN2_TIMER,&timere_initpara);        // ��ʼ����ʱ��
    timer_init(BSP_IN3_TIMER,&timere_initpara);        // ��ʼ����ʱ��
    timer_init(BSP_IN4_TIMER,&timere_initpara);        // ��ʼ����ʱ��
    timer_init(BSP_IN5_TIMER,&timere_initpara);        // ��ʼ����ʱ��
    timer_init(BSP_IN6_TIMER,&timere_initpara);        // ��ʼ����ʱ��
    timer_init(BSP_IN7_TIMER,&timere_initpara);        // ��ʼ����ʱ��
    timer_init(BSP_IN8_TIMER,&timere_initpara);        // ��ʼ����ʱ��

    /* ��������ṹ�� */
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;           // ��Ч��ƽ�ļ���                                                                                                                                  
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;                // ���ñȽ����ģʽ״̬ Ҳ����ʹ��PWM������˿�
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;        // ͨ�������������Ϊ�ߵ�ƽ
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;            // ͨ���������״̬ʧ��
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;        // �ŵ�����Ŀ���״̬Ϊ��
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;// �ŵ���������Ŀ���״̬Ϊ��                

    /* ���ö�ʱ��������� */
    timer_channel_output_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,&timer_ocintpara);

    /* ����ռ�ձ� ��Χ 0 ~ (per-1) */
    timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,0);                                        
    timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,0); 
    timer_channel_output_pulse_value_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,0);                                        
    timer_channel_output_pulse_value_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,0); 


    timer_channel_output_mode_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
    timer_channel_output_mode_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
    timer_channel_output_mode_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
    timer_channel_output_mode_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
    timer_channel_output_mode_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
    timer_channel_output_mode_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
    timer_channel_output_mode_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
    timer_channel_output_mode_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ

    timer_channel_output_shadow_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    timer_channel_output_shadow_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    timer_channel_output_shadow_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    timer_channel_output_shadow_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    timer_channel_output_shadow_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    timer_channel_output_shadow_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    timer_channel_output_shadow_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
    timer_channel_output_shadow_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���

    /* ֻ�и߼���ʱ��ʹ�� */
    timer_auto_reload_shadow_enable(BSP_IN1_TIMER);
    timer_auto_reload_shadow_enable(BSP_IN2_TIMER);
    timer_auto_reload_shadow_enable(BSP_IN3_TIMER);
    timer_auto_reload_shadow_enable(BSP_IN4_TIMER);
    timer_auto_reload_shadow_enable(BSP_IN5_TIMER);
    timer_auto_reload_shadow_enable(BSP_IN6_TIMER);
    timer_auto_reload_shadow_enable(BSP_IN7_TIMER);
    timer_auto_reload_shadow_enable(BSP_IN8_TIMER);

    timer_primary_output_config(BSP_IN1_TIMER, ENABLE);
    timer_primary_output_config(BSP_IN2_TIMER, ENABLE);
    timer_primary_output_config(BSP_IN3_TIMER, ENABLE);
    timer_primary_output_config(BSP_IN4_TIMER, ENABLE);
    timer_primary_output_config(BSP_IN5_TIMER, ENABLE);
    timer_primary_output_config(BSP_IN6_TIMER, ENABLE);
    timer_primary_output_config(BSP_IN7_TIMER, ENABLE);
    timer_primary_output_config(BSP_IN8_TIMER, ENABLE);

    /* ʹ�ܶ�ʱ�� */
    timer_enable(BSP_IN1_TIMER);
    timer_enable(BSP_IN2_TIMER);
    timer_enable(BSP_IN3_TIMER);
    timer_enable(BSP_IN4_TIMER);
    timer_enable(BSP_IN5_TIMER);
    timer_enable(BSP_IN6_TIMER);
    timer_enable(BSP_IN7_TIMER);
    timer_enable(BSP_IN8_TIMER);
}

void set_moto_control(MOTO_NUM moto_num, int16_t speed)
{
    switch(moto_num)
    {
        case MOTO_4:
            speed > 0 ? DO_Control(1, speed) : DO_Control(0, -speed);
            break;
        case MOTO_3:
            speed > 0 ? CO_Control(1, speed) : CO_Control(0, -speed);
            break;
        case MOTO_2:
            speed > 0 ? BO_Control(1, speed) : BO_Control(0, -speed);
            break;
        case MOTO_1:
            speed > 0 ? AO_Control(1, speed) : AO_Control(0, -speed);
            break;
        default:
            break;
    }
}

/******************************************************************
 * �� �� �� �ƣ�AO_Control
 * �� �� ˵ ����A�˿ڵ������
 * �� �� �� �Σ�dir��ת���� 1��ת0��ת   speed��ת�ٶȣ���Χ��0 ~ per-1��
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 ******************************************************************/
void AO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x01;
        //AO1���
        timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL, 0 );        
        //AO2���
        timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0xfe;
        //AO1���
        timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL, speed );        
        //AO2���
        timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL, 0 );        
    }        
}

/******************************************************************
 * �� �� �� �ƣ�B0_Control
 * �� �� ˵ ����B�˿ڵ������
 * �� �� �� �Σ�dir��ת���� 1��ת0��ת   speed��ת�ٶȣ���Χ��0 ~ per-1��
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 ******************************************************************/
void BO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x02;
        //BO1���
        timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, 0 );        
        //BO2���
        timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0xfd;
        //BO1���
        timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, speed );        
        //BO2���
        timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, 0 );        
    }
}

/******************************************************************
 * �� �� �� �ƣ�C0_Control
 * �� �� ˵ ����C�˿ڵ������
 * �� �� �� �Σ�dir��ת���� 1��ת0��ת   speed��ת�ٶȣ���Χ��0 ~ per-1��
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 ******************************************************************/
void CO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x04;
        //CO1���
        timer_channel_output_pulse_value_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL, 0 );        
        //BO2���
        timer_channel_output_pulse_value_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0x0b;
        //CO1���
        timer_channel_output_pulse_value_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL, speed );        
        //CO2���
        timer_channel_output_pulse_value_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL, 0 );        
    }
}

/******************************************************************
 * �� �� �� �ƣ�D0_Control
 * �� �� ˵ ����D�˿ڵ������
 * �� �� �� �Σ�dir��ת���� 1��ת0��ת   speed��ת�ٶȣ���Χ��0 ~ per-1��
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
 ******************************************************************/
void DO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x08;
        //DO1���
        timer_channel_output_pulse_value_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL, 0 );        
        //DO2���
        timer_channel_output_pulse_value_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0x07;
        //DO1���
        timer_channel_output_pulse_value_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL, speed );        
        //DO2���
        timer_channel_output_pulse_value_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL, 0 );        
    }
}
