/**
* @file                framework_pmu.c
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
 * @defgroup 周期与功耗管理模块
 * @brief 控制固件循环周期
 * @details 该模块控制固件循环周期，分为active、lowpower、sleep三种模式，三种模式的周期时间可由开发者配置相关宏定义进行设定
 * @{
 */
//hal
#include "hal_pmu.h"
#include "hal_timer.h"
#include "hal_gpio.h"
//fw
#include "framework_pmu.h"
#include "framework_debug.h"

//config
#include "user_config.h"

static uint16_t pmu_into_lowpower_count = PMU_INTO_LOWPOWER_TIME;

static uint16_t pmu_into_deep_lowpower_count  = PMU_INTO_LOWPOWER_TIME;
static uint16_t pmu_into_sleep_count = PMU_INTO_SLEEP_TIME;
static fw_pmu_run_mode_t pmu_run_mode = PMU_ACTIVE_MODE;
static fw_pmu_run_mode_t pmu_run_mode_bak = PMU_ACTIVE_MODE;
static uint8_t flag_pmu_auto_lowpower = PMU_AUTO_LOWPOWER_ENABLE;
static uint8_t flag_pmu_auto_deep_lowpower = PMU_AUTO_DEEP_LOWPOWER_ENABLE;
static volatile uint8_t pmu_timer_interrput_count = 0;
static uint16_t pmu_period_time;
static volatile uint8_t pmu_exit_waiting_level = 0;     // 0: 不退出 /1: sleep mode 状态下退出， /2: sleep mode & lowpower 状态下退出  3: 所有状态下都退出
static volatile uint8_t flag_pmu_cmd_sleep = 0;

/**
* @fn void fw_pmu_init(void)
* @brief pmu system init
* @param None
* @return None.
*/
void fw_pmu_init(void)                                  // 功耗系统初始化
{
    hal_timer_init(PMU_ACTIVE_MODE_PERIOD_TIME);        // 周期定时器初始化

    flag_pmu_auto_lowpower = PMU_AUTO_LOWPOWER_ENABLE;	//低功耗处理设置：使能进入lowpower运行模式
    pmu_into_lowpower_count = PMU_INTO_LOWPOWER_TIME;	//低功耗处理设置： 2S 自动进入lowpower运行模式时长
    pmu_period_time = PMU_ACTIVE_MODE_PERIOD_TIME;
    pmu_run_mode = PMU_ACTIVE_MODE;						//初始化默认模式：active模式
}

/**
* @fn void fw_pmu_timer_interrput(void)
* @brief timer interrput
* @param None
* @return None.
*/
void fw_pmu_timer_interrput(void)                       // timer 中断调用函数
{
    pmu_timer_interrput_count++;
}


/**
* @fn void fw_pmu_set_run_mode(fw_pmu_run_mode_t mode)
* @brief set run mode
* @param [in]mode: run mode
* @return None.
*/
void fw_pmu_set_run_mode(fw_pmu_run_mode_t mode)        // 设置工作模式，active/lowpo/sleep
{
    pmu_run_mode = mode;

    pmu_into_lowpower_count = PMU_INTO_LOWPOWER_TIME;

    pmu_into_sleep_count = PMU_INTO_SLEEP_TIME;
}

/**
* @fn uint8_t fw_pmu_get_run_mode(void)
* @brief get run mode
* @param None
* @return run mode
*/
fw_pmu_run_mode_t fw_pmu_get_run_mode(void)        			// 获取当前工作模式
{
    return pmu_run_mode;
}

/**
* @fn void fw_pmu_set_sleep_cmd(uint8_t cmd)
* @brief set allow sleep cmd
* @param [in]mode: sleep cmd , 0: not sllep ,1:sleep
* @return None.
*/
void fw_pmu_set_sleep_cmd(uint8_t cmd)                  // 设置允许sleep命令
{
    flag_pmu_cmd_sleep = cmd;
}

/**
* @fn uint16_t fw_pmu_get_period_time(void)
* @brief get period time
* @param None
* @return time data
*/
uint16_t fw_pmu_get_period_time(void)                   // 获取设置周期时间
{
    return pmu_period_time;
}

/**
* @fn static void fw_pmu_set_period_time(uint16_t timer)
* @brief set period time
* @param time data
* @return None.
*/
static void fw_pmu_set_period_time(uint16_t timer)     // 设置周期时间
{
    static uint16_t timer_bak = 0;

    if(timer_bak != timer)
    {

        hal_timer_counter_set(timer);

        timer_bak = timer;

        pmu_timer_interrput_count = 0;
    }
}

/**
* @fn static uint8_t fw_pmu_lowpower_timeout(void)
* @brief cal lowpower count
* @param None
* @return result，0: time is not out, 1:time out
*/
static uint8_t fw_pmu_lowpower_timeout(void)            // 检测进入lowpower模式时间是否到
{
    if(flag_pmu_auto_lowpower)                          // 如果允许自动进入lowpower
    {
        if(pmu_into_lowpower_count)
        {
            pmu_into_lowpower_count--;
        }
        else
        {
            pmu_into_lowpower_count = PMU_INTO_LOWPOWER_TIME;
            return 1;                                   // 时间到，返回1
        }
    }

    return 0;                                           // 时间未到，返回0
}

/**
* @fn static uint8_t fw_pmu_deep_lowpower_timeout(void)
* @brief cal lowpower count
* @param None
* @return result，0: time is not out, 1:time out
*/
static uint8_t fw_pmu_deep_lowpower_timeout(void)
{
    if(flag_pmu_auto_deep_lowpower)                     // 如果允许自动进入lowpower
    {
        if(pmu_into_deep_lowpower_count)
        {
            pmu_into_deep_lowpower_count--;
        }
        else
        {
            pmu_into_deep_lowpower_count = PMU_INTO_LOWPOWER_TIME;
            return 1;                                   // 时间到，返回1
        }
    }

    return 0;                                           // 时间未到，返回0
}

/**
* @fn static uint8_t fw_pmu_sleep_timeout(void)
* @brief cal sleep count
* @param None
* @return  result，0: time is not out, 1:time out
*/
static uint8_t fw_pmu_sleep_timeout(void)               // 检测进入sleep模式时间是否到
{
    if(flag_pmu_cmd_sleep)                              // 如果允许进入sleep
    {
        if(pmu_into_sleep_count){
            pmu_into_sleep_count--;
        }
        else{
            pmu_into_sleep_count = PMU_INTO_SLEEP_TIME;
            return 1;
        }
    }
    return 0;
}

/**
* @fn static void fw_pmu_power_saving(uint8_t save_level)
* @brief power saveing
* @param [in] save_level: level
* @return None.
*/
static void fw_pmu_power_saving(uint8_t save_level)         // 设置周期空闲时间的省电级别
{
    switch(save_level)
    {
    case 1:
        hal_pmu_enter_sleep();
        break;

    case 2:

        hal_pmu_enter_deep_sleep1();
        break;

    case 3:
        hal_pmu_enter_deep_sleep2();
        break;

    default:
        break;
    }
}

/**
* @fn void fw_pmu_set_exit_waiting_level(uint8_t exit_level)
* @brief set out waiting time level
* @param [in]exit_level: level  0: low -> 3: high
* @return None.
*/
void fw_pmu_set_exit_waiting_level(uint8_t exit_level)          //设置提前退出周期等待等级， 中断函数调用，单次有效
{
    if(exit_level > 3)
    {
        exit_level = 3;
    }

    pmu_exit_waiting_level = exit_level;
}

/**
* @fn static uint8_t fw_pmu_exit_waiting_check(void)
* @brief check exit waiting time
* @param None
* @return result,0: go on waiting , 1: exit
*/
static uint8_t fw_pmu_exit_waiting_check(void)                  //检测是否需要退出周期等待
{
    uint8_t flag_out = 0;

    switch(pmu_exit_waiting_level)
    {
    case 1:                                                     // 第1级：在sleep 模式下才退出
        if(pmu_run_mode_bak == PMU_SLEEP_MODE)
        {
            flag_out = 1;
        }
        break;

    case 2:                                                     // 第2级：在sleep 模式或lowpower模式下才退出
        if(pmu_run_mode_bak != PMU_ACTIVE_MODE)
        {
            flag_out = 1;
        }
        break;

    case 3:                                                     // 第3级：在任意模式下都退出
        flag_out = 1;
        break;

    default:
        flag_out = 0;
        break;
    }

    return flag_out;
}

/**
* @fn void fw_pmu_proc(void)
* @brief pmu system proc
* @param None
* @return None.
*/
void fw_pmu_enter_sleep_judge(void)                          // 功耗系统处理函数
{
    static uint8_t io_timer = 0;

    if(hal_gpio_get_int_in_pin())
    {
        if(io_timer > 10)
        {
            fw_pmu_set_run_mode(PMU_SLEEP_MODE);
        }
        io_timer++;
    }
    else
    {
        io_timer = 0;
    }
}


/**
* @fn void fw_pmu_proc(void)
* @brief pmu system proc
* @param None
* @return None.
*/

void fw_pmu_proc(void)                                      		// 功耗系统处理函数
{
    char power_saving_level = 0;

    switch(pmu_run_mode)                                            // 根据当前运行模式选择
    {
    case PMU_ACTIVE_MODE:
        power_saving_level = PMU_ACTIVE_MODE_POWER_SAVING_LEVEL;    // 设置周期等待的省电级别
        pmu_period_time = PMU_ACTIVE_MODE_PERIOD_TIME;              // 设置周期等待时间
        if(fw_pmu_sleep_timeout())                                  // 判断是否需要进入sleep模式
        {
            pmu_run_mode = PMU_SLEEP_MODE;
        }
        if(fw_pmu_lowpower_timeout())                               // 判断是否需要进入lowpower模式
        {
            pmu_run_mode = PMU_STANDARD_MODE;
        }
    break;

    case PMU_STANDARD_MODE:
        power_saving_level = PMU_STANDARD_MODE_POWER_SAVING_LEVEL;    // 设置周期等待的省电级别
        pmu_period_time = PMU_STANDARD_MODE_PERIOD_TIME;              // 设置周期等待时间
        if(fw_pmu_sleep_timeout())                                  // 判断是否需要进入sleep模式
        {
            pmu_run_mode = PMU_SLEEP_MODE;
        }

        break;
    case PMU_LOWPOWER_MODE:
        power_saving_level = PMU_LOWPOWER_MODE_POWER_SAVING_LEVEL;  // 设置周期等待的省电级别

        pmu_period_time = PMU_LOWPOWER_MODE_PERIOD_TIME;            // 设置周期等待时间

        if(fw_pmu_sleep_timeout())                                  // 判断是否需要进入sleep模式
        {
            pmu_run_mode = PMU_SLEEP_MODE;
        }

        break;
    case PMU_SLEEP_MODE:
        power_saving_level = PMU_SLEEP_MODE_POWER_SAVING_LEVEL;     // 设置周期等待的省电级别
        pmu_period_time = PMU_SLEEP_MODE_PERIOD_TIME;               // 设置周期等待时间
        break;
    case PMU_DEEP_LOWPOWER_MODE:
        power_saving_level = PMU_LOWPOWER_MODE_POWER_SAVING_LEVEL;  // 设置周期等待的省电级别

        pmu_period_time = PMU_DEEP_LOWPOWER_MODE_PERIOD_TIME;       // 设置周期等待时间

        if(fw_pmu_sleep_timeout())                                  // 判断是否需要进入sleep模式
        {
            pmu_run_mode = PMU_SLEEP_MODE;
        }
        break;
    default:
        power_saving_level = PMU_ACTIVE_MODE_POWER_SAVING_LEVEL;
        pmu_period_time = PMU_ACTIVE_MODE_PERIOD_TIME;
        break;
    }
    fw_pmu_set_period_time(pmu_period_time);                        // 设置周期等待时间
    pmu_exit_waiting_level = 0;                                     // 提前退出周期等待标记清零
    pmu_run_mode_bak = pmu_run_mode;
    while(pmu_timer_interrput_count == 0)                           // 周期等待
    {
        fw_pmu_power_saving(power_saving_level);                    // 进入功耗节省处理
        if(fw_pmu_exit_waiting_check())                             // 判断是否需要提前退出周期等待
        {
            break;
        }
    }
    pmu_timer_interrput_count = 0;                                  // 周期等待计数值清零
}

/**
  * @}
  */

/**
  * @}
  */




