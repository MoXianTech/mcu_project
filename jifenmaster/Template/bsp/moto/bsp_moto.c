#include "bsp_moto.h"
#include "board.h"


/******************************************************************
 * 函 数 名 称：MOTO_Init
 * 函 数 说 明：PWM配置
 * 函 数 形 参： pre定时器时钟预分频值    per周期
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：PWM频率=168 000 000 /( (pre+1) * (per+1) ) 
 ******************************************************************/


/* moto flag dire  0000, 0,0,0,0 = 4, 3, 2, 1*/
volatile uint8_t moto_dire_flag = 0x00;

void moto_init(uint16_t pre,uint16_t per)
{                             
    timer_parameter_struct timere_initpara={0};                 // 定义定时器结构体
    timer_oc_parameter_struct timer_ocintpara={0};              //定时器比较输出结构体

    //定时器时钟
    rcu_periph_clock_enable(RCU_IN1_TIMER);     // 开启定时器时钟
    rcu_periph_clock_enable(RCU_IN2_TIMER);                        
    rcu_periph_clock_enable(RCU_IN3_TIMER);                        
    rcu_periph_clock_enable(RCU_IN4_TIMER);
    rcu_periph_clock_enable(RCU_IN5_TIMER);     
    rcu_periph_clock_enable(RCU_IN6_TIMER);                        
    rcu_periph_clock_enable(RCU_IN7_TIMER);                        
    rcu_periph_clock_enable(RCU_IN8_TIMER);
    //引脚时钟
    rcu_periph_clock_enable(RCU_IN1);
    rcu_periph_clock_enable(RCU_IN2);
    rcu_periph_clock_enable(RCU_IN3);
    rcu_periph_clock_enable(RCU_IN4);
    rcu_periph_clock_enable(RCU_IN5);
    rcu_periph_clock_enable(RCU_IN6);
    rcu_periph_clock_enable(RCU_IN7);
    rcu_periph_clock_enable(RCU_IN8);

    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);  // 配置定时器时钟

    /* 配置AIN1*/
    gpio_mode_set(PORT_IN1,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN1);
    gpio_output_options_set(PORT_IN1,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN1);
    gpio_af_set(PORT_IN1,AF_IN1,GPIO_IN1);        
    /* 配置AIN2*/
    gpio_mode_set(PORT_IN2,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN2);
    gpio_output_options_set(PORT_IN2,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN2);
    gpio_af_set(PORT_IN2,AF_IN2,GPIO_IN2);            

    /* 配置BIN1*/
    gpio_mode_set(PORT_IN3,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN3);
    gpio_output_options_set(PORT_IN3,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN3);
    gpio_af_set(PORT_IN3,AF_IN3,GPIO_IN3);        
    /* 配置BIN2*/
    gpio_mode_set(PORT_IN4,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN4);
    gpio_output_options_set(PORT_IN4,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN4);
    gpio_af_set(PORT_IN4,AF_IN4,GPIO_IN4);
    /* 配置CIN1*/
    gpio_mode_set(PORT_IN5,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN5);
    gpio_output_options_set(PORT_IN5,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN5);
    gpio_af_set(PORT_IN5,AF_IN5,GPIO_IN5);        
    /* 配置CIN2*/
    gpio_mode_set(PORT_IN6,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN6);
    gpio_output_options_set(PORT_IN6,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN6);
    gpio_af_set(PORT_IN6,AF_IN6,GPIO_IN6);            

    /* 配置DIN1*/
    gpio_mode_set(PORT_IN7,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN7);
    gpio_output_options_set(PORT_IN7,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN7);
    gpio_af_set(PORT_IN7,AF_IN7,GPIO_IN7);        
    /* 配置DIN2*/
    gpio_mode_set(PORT_IN8,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_IN8);
    gpio_output_options_set(PORT_IN8,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_IN8);
    gpio_af_set(PORT_IN8,AF_IN8,GPIO_IN8);        


    /* 配置定时器参数 */
    timer_deinit(BSP_IN1_TIMER);        
    timer_deinit(BSP_IN2_TIMER);
    timer_deinit(BSP_IN3_TIMER);
    timer_deinit(BSP_IN4_TIMER);
    timer_deinit(BSP_IN5_TIMER);        
    timer_deinit(BSP_IN6_TIMER);
    timer_deinit(BSP_IN7_TIMER);
    timer_deinit(BSP_IN8_TIMER);

    // 复位定时器
    timere_initpara.prescaler = pre-1;      // 时钟预分频值     
    timere_initpara.alignedmode = TIMER_COUNTER_EDGE;              // 边缘对齐                
    timere_initpara.counterdirection = TIMER_COUNTER_UP;    // 向上计数                                
    timere_initpara.period = per-1;                                              // 周期     
    timere_initpara.clockdivision = TIMER_CKDIV_DIV1;            // 分频因子
    timere_initpara.repetitioncounter = 0;                  // 重复计数器 0-255

    timer_init(BSP_IN1_TIMER,&timere_initpara);        // 初始化定时器
    timer_init(BSP_IN2_TIMER,&timere_initpara);        // 初始化定时器
    timer_init(BSP_IN3_TIMER,&timere_initpara);        // 初始化定时器
    timer_init(BSP_IN4_TIMER,&timere_initpara);        // 初始化定时器
    timer_init(BSP_IN5_TIMER,&timere_initpara);        // 初始化定时器
    timer_init(BSP_IN6_TIMER,&timere_initpara);        // 初始化定时器
    timer_init(BSP_IN7_TIMER,&timere_initpara);        // 初始化定时器
    timer_init(BSP_IN8_TIMER,&timere_initpara);        // 初始化定时器

    /* 配置输出结构体 */
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;           // 有效电平的极性                                                                                                                                  
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;                // 配置比较输出模式状态 也就是使能PWM输出到端口
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;        // 通道互补输出极性为高电平
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;            // 通道互补输出状态失能
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;        // 信道输出的空闲状态为低
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;// 信道互补输出的空闲状态为低                

    /* 配置定时器输出功能 */
    timer_channel_output_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,&timer_ocintpara);
    timer_channel_output_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,&timer_ocintpara);

    /* 配置占空比 范围 0 ~ (per-1) */
    timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,0);                                        
    timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,0); 
    timer_channel_output_pulse_value_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,0);                                        
    timer_channel_output_pulse_value_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,0);                        
    timer_channel_output_pulse_value_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,0); 


    timer_channel_output_mode_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
    timer_channel_output_mode_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
    timer_channel_output_mode_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
    timer_channel_output_mode_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
    timer_channel_output_mode_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
    timer_channel_output_mode_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
    timer_channel_output_mode_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
    timer_channel_output_mode_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式

    timer_channel_output_shadow_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
    timer_channel_output_shadow_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
    timer_channel_output_shadow_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
    timer_channel_output_shadow_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
    timer_channel_output_shadow_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
    timer_channel_output_shadow_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
    timer_channel_output_shadow_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
    timer_channel_output_shadow_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器

    /* 只有高级定时器使用 */
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

    /* 使能定时器 */
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
 * 函 数 名 称：AO_Control
 * 函 数 说 明：A端口电机控制
 * 函 数 形 参：dir旋转方向 1正转0反转   speed旋转速度，范围（0 ~ per-1）
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void AO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x01;
        //AO1输出
        timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL, 0 );        
        //AO2输出
        timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0xfe;
        //AO1输出
        timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL, speed );        
        //AO2输出
        timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL, 0 );        
    }        
}

/******************************************************************
 * 函 数 名 称：B0_Control
 * 函 数 说 明：B端口电机控制
 * 函 数 形 参：dir旋转方向 1正转0反转   speed旋转速度，范围（0 ~ per-1）
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void BO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x02;
        //BO1输出
        timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, 0 );        
        //BO2输出
        timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0xfd;
        //BO1输出
        timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, speed );        
        //BO2输出
        timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, 0 );        
    }
}

/******************************************************************
 * 函 数 名 称：C0_Control
 * 函 数 说 明：C端口电机控制
 * 函 数 形 参：dir旋转方向 1正转0反转   speed旋转速度，范围（0 ~ per-1）
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void CO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x04;
        //CO1输出
        timer_channel_output_pulse_value_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL, 0 );        
        //BO2输出
        timer_channel_output_pulse_value_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0x0b;
        //CO1输出
        timer_channel_output_pulse_value_config(BSP_IN5_TIMER,BSP_IN5_CHANNEL, speed );        
        //CO2输出
        timer_channel_output_pulse_value_config(BSP_IN6_TIMER,BSP_IN6_CHANNEL, 0 );        
    }
}

/******************************************************************
 * 函 数 名 称：D0_Control
 * 函 数 说 明：D端口电机控制
 * 函 数 形 参：dir旋转方向 1正转0反转   speed旋转速度，范围（0 ~ per-1）
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void DO_Control(uint8_t dir, uint32_t speed)
{
    if( dir == 1 )
    {
        moto_dire_flag |= 0x08;
        //DO1输出
        timer_channel_output_pulse_value_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL, 0 );        
        //DO2输出
        timer_channel_output_pulse_value_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL, speed );                
    }
    else
    {
        moto_dire_flag &= 0x07;
        //DO1输出
        timer_channel_output_pulse_value_config(BSP_IN7_TIMER,BSP_IN7_CHANNEL, speed );        
        //DO2输出
        timer_channel_output_pulse_value_config(BSP_IN8_TIMER,BSP_IN8_CHANNEL, 0 );        
    }
}
