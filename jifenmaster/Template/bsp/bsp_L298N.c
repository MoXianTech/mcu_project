/*
 * ������������Ӳ�������������չ����Ӳ�����Ϲ���ȫ����Դ
 * �����������www.lckfb.com
 * ����֧�ֳ�פ��̳���κμ������⻶ӭ��ʱ����ѧϰ
 * ������̳��club.szlcsc.com
 * ��עbilibili�˺ţ������������塿���������ǵ����¶�̬��
 * ��������׬Ǯ���������й�����ʦΪ����
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-02     LCKFB-yzh    first version
 */
#include "bsp_L298N.h"
#include "board.h"


/******************************************************************
 * �� �� �� �ƣ�L298N_Init
 * �� �� ˵ ����PWM����
 * �� �� �� �Σ� pre��ʱ��ʱ��Ԥ��Ƶֵ    per����
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��PWMƵ��=168 000 000 /( (pre+1) * (per+1) ) 
******************************************************************/
void L298N_Init(uint16_t pre,uint16_t per)
{                             
        timer_parameter_struct timere_initpara={0};                 // ���嶨ʱ���ṹ��
        timer_oc_parameter_struct timer_ocintpara={0};              //��ʱ���Ƚ�����ṹ��
                
        //��ʱ��ʱ��
        rcu_periph_clock_enable(RCU_IN1_TIMER);     // ������ʱ��ʱ��
        rcu_periph_clock_enable(RCU_IN2_TIMER);                        
        rcu_periph_clock_enable(RCU_IN3_TIMER);                        
        rcu_periph_clock_enable(RCU_IN4_TIMER);                        
        //����ʱ��
        rcu_periph_clock_enable(RCU_IN1);
        rcu_periph_clock_enable(RCU_IN2);
        rcu_periph_clock_enable(RCU_IN3);
        rcu_periph_clock_enable(RCU_IN4);
        
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


        /* ���ö�ʱ������ */
        timer_deinit(BSP_IN1_TIMER);        
        timer_deinit(BSP_IN2_TIMER);
        timer_deinit(BSP_IN3_TIMER);
        timer_deinit(BSP_IN4_TIMER);
        
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
        
        /* ����ռ�ձ� ��Χ 0 ~ (per-1) */
		timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,0);                                        
		timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,0);                        
		timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,0);                        
		timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,0);                        

		timer_channel_output_mode_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
		timer_channel_output_mode_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
		timer_channel_output_mode_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ
		timer_channel_output_mode_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,TIMER_OC_MODE_PWM0);                        // ���ö�ʱ��ͨ������Ƚ�ģʽ

		timer_channel_output_shadow_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
		timer_channel_output_shadow_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
		timer_channel_output_shadow_config(BSP_IN1_TIMER,BSP_IN3_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���
		timer_channel_output_shadow_config(BSP_IN2_TIMER,BSP_IN4_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // ���ö�ʱ��ͨ�����Ӱ�ӼĴ���

		/* ֻ�и߼���ʱ��ʹ�� */
		timer_auto_reload_shadow_enable(BSP_IN1_TIMER);
		timer_auto_reload_shadow_enable(BSP_IN2_TIMER);
		timer_auto_reload_shadow_enable(BSP_IN3_TIMER);
		timer_auto_reload_shadow_enable(BSP_IN4_TIMER);

		timer_primary_output_config(BSP_IN1_TIMER, ENABLE);
		timer_primary_output_config(BSP_IN2_TIMER, ENABLE);
		timer_primary_output_config(BSP_IN3_TIMER, ENABLE);
		timer_primary_output_config(BSP_IN4_TIMER, ENABLE);

		/* ʹ�ܶ�ʱ�� */
		timer_enable(BSP_IN1_TIMER);
		timer_enable(BSP_IN2_TIMER);
		timer_enable(BSP_IN3_TIMER);
		timer_enable(BSP_IN4_TIMER);
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
		//AO1���
		timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL, 0 );        
		//AO2���
		timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL, speed );                
    }
    else
    {
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
		//BO1���
		timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, 0 );        
		//BO2���
		timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, speed );                
    }
    else
    {
		//BO1���
		timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, speed );        
		//BO2���
		timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, 0 );        
    }
}


