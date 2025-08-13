#include "pwm.h"

void pwm_init(void)
{
    /* 1. 使能GPIO和TIMER时钟 */
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_TIMER3);
    
    /* 2. 配置GPIO为复用推挽输出 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);

//// 拉高 PA15 和 PB3
//		gpio_bit_set(GPIOA, GPIO_PIN_15);
//		gpio_bit_set(GPIOB, GPIO_PIN_3);
	
    /* 3. 配置TIMER3 */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    
    /* 3.1 定时器基本配置 */
    timer_initpara.prescaler         = 120-1;                          // 120MHz/120MHz = 1MHz
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 4000 -1;                         // 1MHz/5000 = 200Hz PWM频率
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER3, &timer_initpara);
    
    /* 3.2 配置PWM模式 */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    
    /* 通道0 (PB6) */
    timer_channel_output_config(TIMER3, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_0, 0); // 初始占空比50%
    
    /* 通道1 (PB7) */
    timer_channel_output_config(TIMER3, TIMER_CH_1, &timer_ocinitpara);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_1, 0); // 初始占空比0%
    
    /* 通道2 (PB8) */
    timer_channel_output_config(TIMER3, TIMER_CH_2, &timer_ocinitpara);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 0); // 初始占空比50%
		
		/* 通道3 (PB9) */
    timer_channel_output_config(TIMER3, TIMER_CH_3, &timer_ocinitpara);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_3, TIMER_OC_MODE_PWM1);
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, 0); // 初始占空比50%
    /* 4. 使能定时器 */
    timer_auto_reload_shadow_enable(TIMER3);
    timer_enable(TIMER3);
		
//		timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_0, 0);
//		timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_1, 0);
//		timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 0);
//		timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, 0);
		
}
