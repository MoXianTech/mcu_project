/**
* @file                hal_wdt.c
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
 * @defgroup 看门狗驱动
 * @brief 看门狗驱动
 * @details 
 * @{
 */

//driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_iwdt.h"

//hal
#include "hal_wdt.h"

/**
* @fn void hal_wdt_init(void)
* @brief watch dog init
* @param None
* @return None.
*/
void hal_wdt_init(void)                                     // WDT初始化
{
    iwdt_enable();//启动看门狗
}

/**
* @fn void hal_wdt_init(void)
* @brief watch dog init
* @param None
* @return None.
*/
void hal_wdt_init_longest_reset_time(void)                      // WDT初始化:设置为最长复位延时
{
    iwdt_enable();//启动看门狗
}


/**
* @fn void hal_wdt_refresh(void)
* @brief free wdt
* @param None
* @return None.
*/
void hal_wdt_refresh(void)                                  // 喂狗函数
{
    iwdt_counter_reload();
}

/**
  * @}
  */

/**
  * @}
  */
