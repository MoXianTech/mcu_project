/**
* @file                hal_pmu.c
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
 * @defgroup 周期与功耗驱动
 * @brief 周期与功耗驱动
 * @details 
 * @{
 */
//drvier
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_hdiv.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_syscfg.h"
#include "csa37f7x_pwr.h"

//hal
#include "hal_pmu.h"

/**
* @fn void hal_pmu_init(void)
* @brief Pmu manager init
* @param None
* @return None.
*/
void hal_pmu_init(void)                         // 功耗管理初始化
{

}

/**
  * @fn void hal_pmu_enter_sleep(void)
  * @brief	Sleep
  * @param	none
  * @retval none
 */
void hal_pmu_enter_sleep(void)                  // 控制芯片进入sleep
{
    syscfg_regwrprot_disable();
    pwr_sleep_mode_enter(PWR_SLEEP_MODE_NORMAL,PWR_SLEEP_ENTRY_WFI);
    syscfg_regwrprot_enable();
}


/**
* @fn void EnterDeepSleep1(void)
* @brief  Deep sleep mode 1
* @param  none
* @retval none
*/
void hal_pmu_enter_deep_sleep1(void)            // 控制芯片进入深度sleep 1
{
    pwr_interrupt_config(PWR_IT_SLP1_WK,ENABLE);

    syscfg_regwrprot_disable();
    pwr_sleep_mode_enter(PWR_SLEEP_MODE_DEEP1,PWR_SLEEP_ENTRY_WFI);
    syscfg_regwrprot_enable();
}

/**
* @fn void EnterDeepSleep2(void)
* @brief  Deep sleep mode 2
* @param  none
* @retval none
*/
void hal_pmu_enter_deep_sleep2(void)            // 控制芯片进入深度sleep 2
{
    pwr_interrupt_config(PWR_IT_SLP2_WK,ENABLE);

    syscfg_regwrprot_disable();
    pwr_sleep_mode_enter(PWR_SLEEP_MODE_DEEP2,PWR_SLEEP_ENTRY_WFI);
    syscfg_regwrprot_enable();

}

/**
  * @}
  */

/**
  * @}
  */
