/**
* @file                framework_notify.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup Framework层
  * @{
  */
/**
 * @defgroup 通知模块
 * @brief 通知接口模块
 * @details 主要功能为控制GPIO输入、输出； 开发者需根据实际硬件情况指定输入、输出GPIO端口名称； GPIO输出功能包含中断脉冲输出、按键电平输出； GPIO输入功能包含主控中断通知输入；
 * @{
 */
//define


#define BIT(X) ((uint32_t)(1)<<(X))

//hal
#include "hal_timer.h"
#include "hal_gpio.h"

//framework
#include "framework_notify.h"

//config
#include "user_config.h"

//
static uint8_t flag_notify_int_out = 0;                                 //0: disable / 1: enable
static uint8_t notify_key_status;                                       //bit  0:no key  /  1: have key
static uint8_t flag_notify_disable_int = 0;                             //0: default   /  1: disable
static uint8_t flag_notify_disable_key = 0;                             //0: default   /  1: disable
static uint8_t flag_notify_int_in = 0;                                  //0:no interrput  /  1: have interrupt
static uint8_t flag_notify_work = 0;                                  //0:no interrput to work /  1: have interrupt to work
static uint8_t flag_motor_trig = 0;                                     //0: disable  / 1: enable
static uint8_t flag_motor_trig_enable = 1;
/**
* @fn void fw_notify_init(void)
* @brief notify system init
* @param None
* @return None.
*/
void fw_notify_init(void)                   //通知系统初始化
{

#if NOTIFY_INT_OUT_FUNCTION_ENABLE          //  中断输出IO初始化
    hal_gpio_int_out_init();

    fw_notify_int_out_clear();

#if NOTIFY_INT_OUT_DEFAULT_HIGH
    hal_gpio_int_out_high();
#else
    hal_gpio_int_out_low();
#endif
    fw_notify_int_out_enable();
#endif

#if NOTIFY_INT_IN_FUNCTION_ENABLE           //  中断输入IO初始化
    hal_gpio_int_in_init();
    fw_notify_set_int_in_status(0);
#endif

#if NOTIFY_MOTOR_TRIG_ENBALE                // 马达驱动IO初始化
    hal_gpio_motor_init();
    fw_notify_motor_trig_enable();          // 默认使能输出
#endif

#if NOTIFY_KEY_FUNCTION_ENABLE              //按键电平输出IO初始化
    hal_gpio_key_init();

    fw_notify_all_key_out_clear();

    fw_notify_key_out_enable();
#endif
}

/**
* @fn void fw_notify_int_out_enable(void)
* @brief int out enable
* @param None
* @return None.
*/
void fw_notify_int_out_enable(void)         // 允许中断输出IO输出脉冲
{
    flag_notify_disable_int = 0;
}

/**
* @fn void fw_notify_int_out_disable(void)
* @brief int out disable
* @param None
* @return None.
*/
void fw_notify_int_out_disable(void)        // 禁止中断输出IO输出脉冲
{
    flag_notify_disable_int = 1;
}

/**
* @fn void fw_notify_key_out_enable(void)
* @brief key io out enable
* @param None
* @return None.
*/
void fw_notify_key_out_enable(void)         // 允许按键输出IO输出按压电平
{
    flag_notify_disable_key = 0;
}

/**
* @fn void fw_notify_key_out_disable(void)
* @brief key io out disable
* @param None
* @return None.
*/
void fw_notify_key_out_disable(void)        // 禁止按键输出IO输出按压电平
{
    flag_notify_disable_key = 1;
}

/**
* @fn void fw_notify_int_out_once(void)
* @brief int out once
* @param None
* @return None.
*/
void fw_notify_int_out_once(void)           // 通知中断IO输出一次脉冲，中断输出允许的前提下
{
    flag_notify_int_out = 1;
}

/**
* @fn void fw_notify_int_out_clear(void)
* @brief int out flag clear
* @param None
* @return None.
*/
static void fw_notify_int_out_clear(void)   // 清除通知中断IO输出脉冲标记
{
    flag_notify_int_out = 0;
}

/**
* @fn void fw_notify_all_key_out_clear(void)
* @brief key out status clear
* @param None
* @return None.
*/
static void fw_notify_all_key_out_clear(void)      // 清除所有按键状态有效标记
{
    notify_key_status = 0;
}

/**
* @fn void fw_notify_set_key_status(fw_noticy_key_num_t key_num, fw_noticy_key_stauts_t key_status)
* @brief set key out status
* @param [in]key_num: key num, [in]key_status:key status
* @return None.
*/
void fw_notify_set_key_status(fw_noticy_key_num_t key_num, fw_noticy_key_stauts_t key_status) // 设置按键按压状态
{
    if(key_status == NOTICY_KEY_DOWN)
    {
        notify_key_status |= BIT(key_num);
    }
    else
    {
        notify_key_status &= ~BIT(key_num);
    }
}

/**
* @fn fw_notify_set_key_pin(uint8_t num, uint8_t status)
* @brief set key pin stauts
* @param None
* @return None.
*/
static void fw_notify_set_key_pin(uint8_t num, uint8_t status)      // 设置按键IO电平状态
{
    switch(num)
    {
    case 0:
        hal_gpio_set_key_pin(HAL_GPIO_KEY0, status);
        break;

    case 1:
        hal_gpio_set_key_pin(HAL_GPIO_KEY1, status);
        break;

    case 2:
        hal_gpio_set_key_pin(HAL_GPIO_KEY2, status);
        break;

    case 3:
        hal_gpio_set_key_pin(HAL_GPIO_KEY3, status);
        break;

    default:
        break;
    }
}

/**
* @fn fw_notify_set_int_in_status(uint8_t status)
* @brief set int in flag stauts
* @param [in]status: status,0:none , 1:have
* @return None.
*/
void fw_notify_set_int_in_status(uint8_t status)        // 设置输入中断标记状态
{
    flag_notify_int_in = status;
}

/**
* @fn uint8_t fw_notify_get_int_in_status(void)
* @brief get int in flag status
* @param None
* @return int in flag status
*/
uint8_t fw_notify_get_int_in_status(void)               // 获取输入中断标记状态
{
    uint8_t temp = flag_notify_int_in;

    flag_notify_int_in = 0;

    return temp;
}
/**
* @fn fw_notify_set_work_status(uint8_t status)
* @brief set int in flag stauts
* @param [in]status: status,0:none , 1:have
* @return None.
*/
void fw_notify_set_work_status(uint8_t status)        // 设置输入中断标记状态
{
    flag_notify_work = status;
}

/**
* @fn uint8_t fw_notify_get_work_status(void)
* @brief get int in flag status
* @param None
* @return int in flag status
*/
uint8_t fw_notify_get_work_status(void)               // 获取输入中断标记状态
{
    uint8_t temp = flag_notify_work;

    flag_notify_work = 0;

    return temp;
}
/**
* @fn uint8_t fw_notify_get_int_in_pin_status(void)     // 获取输入中断引脚电平状态
* @brief get int in pin status
* @param None
* @return None.
*/
uint8_t fw_notify_get_int_in_pin_status(void)
{
    return hal_gpio_get_int_in_pin();
}

/**
* @fn void fw_notify_int_out_once(void)
* @brief int out once
* @param None
* @return None.
*/
void fw_notify_set_motor_trig_once(void)                    // 通知输出一次马达震动，马达震动允许的前提下
{
    flag_motor_trig = 1;
}

/**
* @fn void fw_notify_int_out_once(void)
* @brief int out once
* @param None
* @return None.
*/
void fw_notify_motor_trig_enable(void)                    // 通知输出一次马达震动，马达震动允许的前提下
{
    flag_motor_trig_enable = 1;
}

/**
* @fn void fw_notify_int_out_once(void)
* @brief int out once
* @param None
* @return None.
*/
void fw_notify_motor_trig_disable(void)                    // 通知输出一次马达震动，马达震动允许的前提下
{
    flag_motor_trig_enable = 0;
}

/**
* @fn void fw_notify_int_out_once(void)
* @brief int out once
* @param None
* @return None.
*/
static void fw_notify_motor_trig_proc(void)                    // 通知输出一次马达震动，马达震动允许的前提下
{
    static uint8_t io_status = 0;

    if(flag_motor_trig && flag_motor_trig_enable)
    {
        if(io_status)
        {
            hal_gpio_set_motor0_high();
            hal_timer_delay_ms(NOTIFY_MOTOR_INTERVAL_TIME);
            hal_gpio_set_motor1_high();
            io_status = 0;
        }
        else
        {
            hal_gpio_set_motor0_low();
            hal_timer_delay_ms(NOTIFY_MOTOR_INTERVAL_TIME);
            hal_gpio_set_motor1_low();
            io_status = 1;
        }
    }
    flag_motor_trig = 0;
}



void fw_notify_proc(void)                                       // 通知系统处理函数
{
	
}