/**
* @file                main.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details             Program start and main function.
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup APP层
  * @{
  */

/** @defgroup main函数
  * @brief 主程序入口
  * @{
  */
//framework
#include "hal_temperature.h"
#include "hal_timer.h"
#include "framework_system.h"
#include "framework_debug.h"
#include "framework_notify.h"
#include "framework_pmu.h"
#include "framework_reg.h"
#include "framework_sample.h"
#include "framework_sample_tk.h"
#include "framework_sample_all.h"
#include "absacc.h"

/* 若生成固件用于升级请屏蔽这段代码 */
const unsigned int UPDATE_OK_FLAG __at(0x0000EE00) = 0x6E727562;  //手动写入固件完整标记


/**
  * @fn  int main(void)
  * @brief  Main function
  * @param
  * @return None
  */
int main(void)
{

    fw_system_init();
	
    while(1)
    {
		
        fw_system_check();
        #if 0
        fw_tk_sample_proc();
        fw_sample_proc();
        #else
        fw_sample_proc_all();
        #endif

#if DEBUG_PRINT_EN
        fw_sample_data_print();
		hal_timer_delay_ms(20);
#endif
        fw_notify_proc();
		
        fw_reg_proc();
	
        fw_pmu_proc();	

    }
	
}


/**
  * @}
  */
/**
  * @}
  */
