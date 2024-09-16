/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：club.szlcsc.com
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-02     LCKFB-yzh    first version
 */
#include "bsp_L298N.h"
#include "board.h"


/******************************************************************
 * 函 数 名 称：L298N_Init
 * 函 数 说 明：PWM配置
 * 函 数 形 参： pre定时器时钟预分频值    per周期
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：PWM频率=168 000 000 /( (pre+1) * (per+1) ) 
******************************************************************/
void L298N_Init(uint16_t pre,uint16_t per)
{                             
        timer_parameter_struct timere_initpara={0};                 // 定义定时器结构体
        timer_oc_parameter_struct timer_ocintpara={0};              //定时器比较输出结构体
                
        //定时器时钟
        rcu_periph_clock_enable(RCU_IN1_TIMER);     // 开启定时器时钟
        rcu_periph_clock_enable(RCU_IN2_TIMER);                        
        rcu_periph_clock_enable(RCU_IN3_TIMER);                        
        rcu_periph_clock_enable(RCU_IN4_TIMER);                        
        //引脚时钟
        rcu_periph_clock_enable(RCU_IN1);
        rcu_periph_clock_enable(RCU_IN2);
        rcu_periph_clock_enable(RCU_IN3);
        rcu_periph_clock_enable(RCU_IN4);
        
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


        /* 配置定时器参数 */
        timer_deinit(BSP_IN1_TIMER);        
        timer_deinit(BSP_IN2_TIMER);
        timer_deinit(BSP_IN3_TIMER);
        timer_deinit(BSP_IN4_TIMER);
        
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
        
        /* 配置占空比 范围 0 ~ (per-1) */
		timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,0);                                        
		timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,0);                        
		timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,0);                        
		timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,0);                        

		timer_channel_output_mode_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
		timer_channel_output_mode_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
		timer_channel_output_mode_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式
		timer_channel_output_mode_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL,TIMER_OC_MODE_PWM0);                        // 配置定时器通道输出比较模式

		timer_channel_output_shadow_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
		timer_channel_output_shadow_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
		timer_channel_output_shadow_config(BSP_IN1_TIMER,BSP_IN3_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器
		timer_channel_output_shadow_config(BSP_IN2_TIMER,BSP_IN4_CHANNEL,TIMER_OC_SHADOW_DISABLE);  // 配置定时器通道输出影子寄存器

		/* 只有高级定时器使用 */
		timer_auto_reload_shadow_enable(BSP_IN1_TIMER);
		timer_auto_reload_shadow_enable(BSP_IN2_TIMER);
		timer_auto_reload_shadow_enable(BSP_IN3_TIMER);
		timer_auto_reload_shadow_enable(BSP_IN4_TIMER);

		timer_primary_output_config(BSP_IN1_TIMER, ENABLE);
		timer_primary_output_config(BSP_IN2_TIMER, ENABLE);
		timer_primary_output_config(BSP_IN3_TIMER, ENABLE);
		timer_primary_output_config(BSP_IN4_TIMER, ENABLE);

		/* 使能定时器 */
		timer_enable(BSP_IN1_TIMER);
		timer_enable(BSP_IN2_TIMER);
		timer_enable(BSP_IN3_TIMER);
		timer_enable(BSP_IN4_TIMER);
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
		//AO1输出
		timer_channel_output_pulse_value_config(BSP_IN1_TIMER,BSP_IN1_CHANNEL, 0 );        
		//AO2输出
		timer_channel_output_pulse_value_config(BSP_IN2_TIMER,BSP_IN2_CHANNEL, speed );                
    }
    else
    {
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
		//BO1输出
		timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, 0 );        
		//BO2输出
		timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, speed );                
    }
    else
    {
		//BO1输出
		timer_channel_output_pulse_value_config(BSP_IN3_TIMER,BSP_IN3_CHANNEL, speed );        
		//BO2输出
		timer_channel_output_pulse_value_config(BSP_IN4_TIMER,BSP_IN4_CHANNEL, 0 );        
    }
}


