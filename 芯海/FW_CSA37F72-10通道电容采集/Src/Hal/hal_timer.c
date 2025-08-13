/**
* @file                hal_timer.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup HAL层
  * @{
  */
/**
 * @defgroup 定时器
 * @brief 定时器
 * @details 
 * @{
 */

//standard
#include <stdint.h>

//driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_tim1.h"
#include "csa37f7x_syscfg.h"
#include "csa37f7x_misc.h"

//hal
#include "hal_timer.h"

#define SYSTICK_SOURCE_VALUE    (HRC_VALUE/128)	//<! HCLK = HIRC(24MHz)/HDIV

void systick_init(void)
{
    rcc_hclk_config(RCC_HCLK_DIV128);//!< hclk = 24MHz/div = 12MHz
    SysTick_Config(SYSTICK_SOURCE_VALUE / 1000);//<! period = 1000Hz
}
/**
* @fn       void timer1_init (timer1_config_t *p_timer1_config)
* @brief    enable  systick timer.
* @param    none
* @return   none
*/
void systick_enable(void)
{
    //reg_bit_mask_set(SysTick->CTRL, SYST_CSR_ENABLE, ENABLE);
}
/**
* @fn       void systick__disable (void)
* @brief    disable systick timer.
* @param    none
* @return   none
*/
void systick_disable(void)
{
    //reg_bit_mask_set(SysTick->CTRL, SYST_CSR_ENABLE, DISABLE);
}


/**
* @fn       void systick_status_read (void)
* @brief    read systick timer status.
* @param    none
* @return   none
*/
uint32_t systick_status_read(void)
{
    return SysTick->CTRL;
}

/**
* @fn       uint32_t systick_reload_get (void)
* @brief    get systick reload value.
* @param    none
* @return   systick reload value
*/
uint32_t systick_reload_get(void)
{
    //return reg_bit_mask_get(SysTick->LOAD, SYST_RVR_RELOAD);
    return 0;
}
/**
* @fn       void systick_reload_set  (uint32_t reload_value)
* @brief    set systick reload value.
* @param    systick reload value
* @return   none
*/
void systick_reload_set(uint32_t reload_value)
{
    //reg_bit_mask_set(SysTick->LOAD, SYST_RVR_RELOAD, reload_value);
}
/**
* @fn       uint8_t systick_countflag_get (void)
* @brief    get systick count flag.
* @param    none
* @return   systick count flag
*/
uint8_t systick_countflag_get(void)
{
    //return reg_bit_mask_get(SysTick->CTRL, SYST_CSR_COUNTFLAG);
    return 0;
}


/**
* @fn       void systick_current_value_reset(void)
* @brief    clr systick current value.
* @param    none
* @return   systick count flag
*/
void systick_current_value_reset(void)
{
    SysTick->VAL = 0x00;
}

static uint8_t  fac_us=0;//us延时倍乘数
static uint16_t fac_ms=0;//ms延时倍乘数,在os下,代表每个节拍的ms数
static uint16_t xms_max=0;//记录delay_xms函数输入参数的最大值

/**
* @fn void hal_timer_delay_init(void)
* @brief delay timer init
* @param None
* @return None.
*/
void hal_timer_delay_init(void)                                 // 延时功能定时器初始化
{
    rcc_ahb_periph_clock_enable_ctrl(RCC_AHBPeriph_SYSTICK,ENABLE);
    systick_clock_config(SYSTICK_CLKSEL_HCLK);//HCLK
    fac_us = 24;
    fac_ms = (uint16_t)fac_us*1000;
    xms_max = 0x00ffffff/fac_us/1000;
}

/**
* @fn void hal_timer_delay_ms(int time_ms)
* @brief delay timer fun
* @param [in]time_ms: time, ms
* @return None.
*/
static void hal_timer_delay_xms(uint16_t nms)
{
    uint32_t temp;

    SysTick->LOAD=(uint32_t)nms*fac_ms;     //时间加载(SysTick->LOAD为24bit)
    SysTick->VAL =0x00;                     //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;//开始倒数
    do
    {
        temp=SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));   //等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//关闭计数器
    SysTick->VAL =0X00;                     //清空计数器
}

/**
* @fn void hal_timer_delay_ms(int time_ms)
* @brief delay timer fun
* @param [in]time_ms: time, ms
* @return None.
*/
void hal_timer_delay_ms(int time_ms)      // 定时延时函数，单位ms
{
    uint8_t repeat = time_ms/xms_max;     //计算需要循环多少次delay_xms
    uint16_t remain = time_ms%xms_max;

    while(repeat)
    {
        hal_timer_delay_xms(xms_max);
        repeat--;
    }
    if(remain)
    {
        hal_timer_delay_xms(remain);
    }
}

/**
* @fn void hal_timer_delay_us(int time_us)
* @brief delay timer fun
* @param [in]time_ms: time, ms
* @return None.
*/
void hal_timer_delay_us(int time_us)                            // 定时延时函数，单位us
{
    uint32_t temp;

    SysTick->LOAD=time_us*fac_us; //时间加载
    SysTick->VAL =0x00;  //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒数
    do
    {
        temp=	SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));       //等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;    //关闭计数器
    SysTick->VAL =0X00;                         //清空计数器
}

/**
* @fn void hal_timer_init(uint16_t timer)
* @brief period timer init
* @param None
* @return None.
*/
void hal_timer_init(uint16_t timer)                             // 周期定时器初始化， timer*1ms
{
    tim1_config_t timer1_config;
    nvic_config_t nvic_config;

    /*时钟源选择低速2k时钟*/
    rcc_tim1clk_config(RCC_TIM1CLK_SOURCE_LIRC);
    /*使能定时器1时钟*/
    rcc_apb_periph_clock_enable_ctrl(RCC_APBPeriph_TIM1, ENABLE);

    /*定时器1中断使能*/
    nvic_config.nvic_channel_priority = 0x00;
    nvic_config.nvic_enable_flag = ENABLE;
    nvic_config.nvic_IRQ_channel = IRQn_TIM1;
    nvic_init(&nvic_config);

    /************定时器1配置****************/
    /*定时器1时钟分频为2（默认加一），单个计数时长1ms */
    timer1_config.prescaler = 1;
    /*周期模式选择使能 */
    timer1_config.periodic_sel = DISABLE;
    /*定时器比较值，用于调节进入中断时间：0x00 to oxFFFFFF */
    timer1_config.compare = timer;
    /*定时器1工作模式为周期模式*/
    timer1_config.mode = TIM1_COUNTER_MODE_PERIOD;
    /*timer1初始化*/
    tim1_init(&timer1_config);
    /*timer1 中断使能*/
    tim1_interrupt_enable_ctrl(ENABLE);
    /*设置timer1 cmp值*/
    tim1_compare_set(timer);
    /*定时器1使能*/

    while(tim1_flag_status_get(TIM1_FLAG_RST));//低频时钟计数器硬件复位有延时，复位成功后再使能定时器

    tim1_enable_ctrl(ENABLE);
}


/**
* @fn void hal_timer_counter_set(void)
* @brief 
* @param None
* @return None.
*/
void hal_timer_counter_set(uint16_t timer)
{
    tim1_compare_set(timer);
}

/**
  * @}
  */

/**
  * @}
  */
