#include "bsp_encoder.h"
#include "board.h"
#include "bsp_led.h"

//typedef struct
//{
//	uint8_t currentCount;//当前计数
//	uint32_t current_tick;//当前脉冲时系统定时器的时间
//	uint32_t tick;//前一个脉冲时系统定时器的时间
//	uint32_t speed;//编码器速度
//	uint8_t lock_flag;//编码器计数的变量保护标志
//}encoder_para_stu;
//encoder_para_stu encoder_para[MOTO_MAX_NUM] = {0};

extern volatile uint8_t moto_dire_flag;
volatile int16_t currentCount[MOTO_MAX_NUM] = {0};//当前计数
uint32_t encoder_current_tick[MOTO_MAX_NUM] = {0};//当前脉冲时系统定时器的计数
uint32_t encoder_tick[MOTO_MAX_NUM] = {0};//前一个脉冲时系统定时器的技术
uint32_t encoder_speed[MOTO_MAX_NUM] = {0};//编码器速度
uint8_t lock_flag = 0;//变量保护标志


/******************************************************************
 * 函 数 名 称：ENCODER_Init
 * 函 数 说 明：定时器配置，输入io配置
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：BS
 * 备       注：无
 ******************************************************************/
void encoder_init(void)
{

    rcu_periph_clock_enable(RCU_ENCODER_A);
    rcu_periph_clock_enable(RCU_ENCODER_B);
    rcu_periph_clock_enable(RCU_ENCODER_C);
    rcu_periph_clock_enable(RCU_ENCODER_D);
    rcu_periph_clock_enable(RCU_SYSCFG);

    gpio_mode_set(PORT_ENCODER_A, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_ENCODER_A);
    gpio_mode_set(PORT_ENCODER_B, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_ENCODER_B);
    gpio_mode_set(PORT_ENCODER_C, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_ENCODER_C);
    gpio_mode_set(PORT_ENCODER_D, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_ENCODER_D);

    nvic_irq_enable(ENCODER_A_EXIT_IRQn, 3U, 3U);
    nvic_irq_enable(ENCODER_B_EXIT_IRQn, 3U, 3U);
    nvic_irq_enable(ENCODER_C_EXIT_IRQn, 3U, 3U);
    nvic_irq_enable(ENCODER_D_EXIT_IRQn, 3U, 3U);


    syscfg_exti_line_config(ENCODER_A_EXTI_PORT_SOURCE, ENCODER_A_EXTI_PIN_SOURCE);
    syscfg_exti_line_config(ENCODER_B_EXTI_PORT_SOURCE, ENCODER_B_EXTI_PIN_SOURCE);
    syscfg_exti_line_config(ENCODER_C_EXTI_PORT_SOURCE, ENCODER_C_EXTI_PIN_SOURCE);
    syscfg_exti_line_config(ENCODER_D_EXTI_PORT_SOURCE, ENCODER_D_EXTI_PIN_SOURCE);


    exti_init(ENCODER_A_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(ENCODER_B_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(ENCODER_C_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_init(ENCODER_D_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);

    exti_interrupt_enable(ENCODER_A_EXTI_LINE);
    exti_interrupt_enable(ENCODER_B_EXTI_LINE);
    exti_interrupt_enable(ENCODER_C_EXTI_LINE);
    exti_interrupt_enable(ENCODER_D_EXTI_LINE);

    exti_interrupt_flag_clear(ENCODER_A_EXTI_LINE);
    exti_interrupt_flag_clear(ENCODER_B_EXTI_LINE);
    exti_interrupt_flag_clear(ENCODER_C_EXTI_LINE);
    exti_interrupt_flag_clear(ENCODER_D_EXTI_LINE);

}

/******************************************************************
 * 函 数 名 称：get_ENCODER_speed(encoder_num encoder)
 * 函 数 说 明：获取编码器速度
 * 函 数 形 参：无
 * 函 数 返 回：每秒多少个脉冲
 * 作       者：BS
 * 备       注：检测方法：T法，即周期测量法，对两个上升沿之间的时间进行测量从而得出速度
 ******************************************************************/
uint32_t get_encoder_speed(moto_num encoder)
{

    uint32_t ret = 0;
    if(get_system_tick()-encoder_tick[encoder]>=1000)//检测是否超时
    {
        ret = 0;
        encoder_tick[encoder] = 0;
        encoder_speed[encoder] = 0;
    }
    else
    {
        ret = encoder_speed[encoder];
    }

    return ret;
}

void set_encoder_currentCount(moto_num encoder, int16_t current_count)
{
    currentCount[encoder] = current_count;
}

/******************************************************************
 * 函 数 名 称：get_ENCODER_currentCount(encoder_num encoder)
 * 函 数 说 明：获取编码器计数
 * 函 数 形 参：无
 * 函 数 返 回：脉冲计数
 * 作       者：BS
 * 备       注：无
 ******************************************************************/
int16_t get_encoder_currentCount(moto_num encoder)
{
    uint32_t ret = 0;
    ret = currentCount[encoder];
    return ret;
}

void lock_encoder_currentCount(moto_num encoder)
{
    switch (encoder)
    {
        case MOTO_A:
            lock_flag |= 0x01;
            break;
        case MOTO_B:
            lock_flag |= 0x02;
            break;
        case MOTO_C:
            lock_flag |= 0x04;
            break;
        case MOTO_D:
            lock_flag |= 0x08;
            break;
        case MOTO_MAX_NUM:
            lock_flag = 0x0F;
            break;
        default:
            break;
    }
}


void unlock_encoder_currentCount(moto_num encoder)
{
    switch (encoder)
    {
        case MOTO_A:
            lock_flag &= 0x0E;
            break;
        case MOTO_B:
            lock_flag &= 0x0D;
            break;
        case MOTO_C:
            lock_flag &= 0x0B;
            break;
        case MOTO_D:
            lock_flag &= 0x07;
            break;
        case MOTO_MAX_NUM:
            lock_flag = 0x00;
            break;
        default:
            break;
    }
}


/******************************************************************
 * 函 数 名 clear_ENCODER_currentCount(encoder_num encoder)
 * 函 数 说 明：清除编码器计数
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：BS
 * 备       注：无
 ******************************************************************/
void clear_encoder_currentCount(moto_num encoder)
{
    lock_flag = 0xff;//变量保护
    currentCount[encoder] = 0;
    lock_flag = 0x00;
}


void EXTI10_15_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(ENCODER_A_EXTI_LINE))
    {
        if((lock_flag & 0x01) == 0)
        {
            (moto_dire_flag & 0x01) ? currentCount[MOTO_A] ++: currentCount[MOTO_A] --;
        }
        exti_interrupt_flag_clear(ENCODER_A_EXTI_LINE);
    }

    if (RESET != exti_interrupt_flag_get(ENCODER_B_EXTI_LINE))
    {
        if((lock_flag & 0x02) == 0)
        {
            (moto_dire_flag & 0x02) ? currentCount[MOTO_B] ++: currentCount[MOTO_B] --;
        }
        exti_interrupt_flag_clear(ENCODER_B_EXTI_LINE);
    }

    if (RESET != exti_interrupt_flag_get(EXTI_13))
    {
        exti_interrupt_flag_clear(EXTI_13);
        lock_encoder_currentCount(MOTO_MAX_NUM);
        bsp_led_on(LED1);
    }

}

/*!
  \brief      this function handles external lines 0 interrupt request
  \param[in]  none
  \param[out] none
  \retval     none
  */
void EXTI5_9_IRQHandler(void)
{
    if (RESET != exti_interrupt_flag_get(ENCODER_C_EXTI_LINE))
    {
        if((lock_flag & 0x04) == 0)
        {
            (moto_dire_flag & 0x04) ? currentCount[MOTO_C] ++: currentCount[MOTO_C] --;
        }
        exti_interrupt_flag_clear(ENCODER_C_EXTI_LINE);
    }

    if (RESET != exti_interrupt_flag_get(ENCODER_D_EXTI_LINE))
    {
        if((lock_flag & 0x08) == 0)
        {
            (moto_dire_flag & 0x08) ? currentCount[MOTO_D] ++: currentCount[MOTO_D] --;
        }

        exti_interrupt_flag_clear(ENCODER_D_EXTI_LINE);
    }

}

//void ENCODER_TIM_Config(uint16_t pre, uint16_t per)
//{
//		/* 开启时钟 */
//		rcu_periph_clock_enable(ENCODER_TIMER_RCU);
//
//		rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4); // 配置定时器时钟
//
//		/* 复位定时器 */
//		timer_deinit(ENCODER_TIMER);
//
//		timer_parameter_struct timer_initpara; 				// 定义定时器结构体
//		/* 配置定时器参数 */
//		timer_initpara.prescaler = pre -1; 					//  时钟预分频值 0-65535  psc_clk = CK_TIMER / pre
//		timer_initpara.alignedmode = TIMER_COUNTER_EDGE; 	// 边缘对齐
//		timer_initpara.counterdirection = TIMER_COUNTER_UP; // 向上计数
//		timer_initpara.period = per  - 1; 					// 周期
//		timer_initpara.clockdivision = TIMER_CKDIV_DIV1; 	// 分频因子
//		timer_initpara.repetitioncounter = 0; 				// 重复计数器 0-255
//		timer_init(ENCODER_TIMER,&timer_initpara); 				// 初始化定时器
//
//		/* 配置中断优先级 */
//		nvic_irq_enable(ENCODER_TIMER_IRQ, 3, 2); // 设置中断优先级为 3,2断
//
//		/* 使能中断 */
//		timer_interrupt_enable(ENCODER_TIMER, TIMER_INT_UP); // 使能更新事件中断
//
//		/* 使能定时器 */
//		timer_enable(ENCODER_TIMER);
//}


