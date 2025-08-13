/**
* @file                interrupt.c
* @brief
* @author
* @version             V0.1.0
* @date                2018.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup APP层
  * @{
  */

/** @defgroup 中断处理
  * @brief 中断处理程序入口
  * @{
  */
//framework
#include "framework_pmu.h"
#include "framework_reg.h"
#include "framework_notify.h"
#include "framework_pmu.h"
#include "framework_debug.h"
#include "framework_system.h"
#include "framework_sample_all.h"
//hal
#include "hal_afe.h"
#include "hal_tk.h"
#include "hal_iic.h"
#include "hal_gpio.h"
//driver
#include "csa37f7x.h"
#include "csa37f7x_tim1.h"
#include "csa37f7x_i2c.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_tk.h"
#include "csa37f7x_reg.h"
//-----------------------------------------------------------------------------
// Interrupt Server function
//-----------------------------------------------------------------------------
//NMI_Handler
void NMI_Handler(void)
{


}

//HardFault_Handler
void HardFault_Handler(void)
{


}

//SVC_Handler
void SVC_Handler(void)
{


}

//PendSV_Handler
void PendSV_Handler(void)
{


}

//SysTick_Handler
void SysTick_Handler(void)
{


}

//BOD_IRQHandler
void BOD_IRQHandler(void)
{


}

//WDT_IRQHandler
void WDT_IRQHandler(void)
{


}

//TMR0_IRQHandler
void TMR0_IRQHandler(void)
{

}

//TMR1_IRQHandler
void TIM1_IRQHandler(void)
{
    tim1_interrupt_flag_clear();
    fw_pmu_timer_interrput();
    fw_system_time_intertupt();
}


//UART0_IRQHandler
void UART0_IRQHandler(void)
{

}

//I2C0_IRQHandler
void I2C_IRQHandler(void)
{
    uint32_t ret  = 0xFF;
    static uint16_t data = 0xFFFF;
    static int8_t cache_num;

    if(i2c_flag_status_get(I2C_STS_ADRF))    // 地址匹配中断关闭
    {
        i2c_flag_clear(I2C_STSC_ADRFC);
    }

    if(i2c_flag_status_get(I2C_STS_TXINTF))
    {
        if(I2C_wPointer < (REG_TABLE[Register].Cmd_Buf_Size))
        {
            if(cache_num <= 0)
            {
                data = 0xFFFF;
                data = REG_TABLE[Register].p_Cmd_Buf[I2C_wPointer + 1];
                data <<= 8;
                data |= REG_TABLE[Register].p_Cmd_Buf[I2C_wPointer];
                cache_num  = 1;
            }
            else
            {
                cache_num--;
                data >>= 8;
            }
            I2C_wPointer ++;
        }
        else
        {
            data = 0xFFFF;
        }
        i2c_data_send(data);
    }
    if(i2c_flag_status_get(I2C_STS_RXNEF)) // 1 byte received
    {
        ret = i2c_data_receive();
        cache_num  = 0;
        if(I2C_AddrFlag)                 //reg addr
        {
            I2C_AddrFlag = NO;
            Register = ret;
            if(Register >= (uint8_t)REG_TABLE_NUM)
            {
                Register = 0;
            }
            if(REG_TABLE[Register].Cmd_Type == REG_OP_COMMAND)
            {
                fw_reg_cmd_funciton(REG_TABLE[Register].Cmd_Id);
            }
            I2C_rPointer = 0;
        }
        else
        {
            if(REG_TABLE[Register].Cmd_Type != REG_OP_READ_ONLY)
            {
                if( I2C_rPointer < (REG_TABLE[Register].Cmd_Buf_Size))       //write data to reg
                {
                    REG_TABLE[Register].p_Cmd_Buf[I2C_rPointer++] = ret;
                }
            }
        }
    }
    if(i2c_flag_status_get(I2C_STS_STOPF))              // STOP
    {
        i2c_flag_clear(I2C_STSC_STOPFC);
        I2C_AddrFlag = YES;
        I2C_rPointer = 0;
        I2C_wPointer = 0;
        cache_num  = 0;
    }
    i2c_flag_clear(I2C_STSC_NACKFC);
    fw_pmu_set_run_mode(PMU_ACTIVE_MODE);   //设置avtive模式
    fw_pmu_set_exit_waiting_level(2);       //设置退出周期等待级别

    hal_iic_clear_busy_count();

    fw_system_set_iic_flag();
}

//AFE0_IRQHandler
void AFE2_IRQHandler(void)
{
    hal_afe_interrput();
    hal_tk_interrput();
    fw_all_afe_interrput();
}

//PA_IRQHandler
void GPIOA_IRQHandler(void)
{
    fw_pmu_timer_interrput();
    gpio_interrupt_flag_clear(GPIO_GROUP_A,GPIO_PIN_NUM11);     //Çå³ýIIC-SDAÏÂ½µÑØÖÐ¶Ï±ê¼Ç
    hal_gpio_get_int_in_int_state_clear();
    if(hal_gpio_get_int_in_pin()==0)
    {
        fw_notify_set_int_in_status(1);
        fw_pmu_set_run_mode(PMU_ACTIVE_MODE);   //设置avtive模式
        fw_pmu_set_exit_waiting_level(2);       //设置退出周期等待级别
    }
}

//PD_WU_IRQHandler
void PD_WU_IRQHandler(void)
{

}

//EINT0_IRQHandler
void EINT0_IRQHandler(void)
{

}

//EINT1_IRQHandler
void EINT1_IRQHandler(void)
{

}

//fault_IRQHandler
void fault_IRQHandler(void)
{

}
//Default_IRQHandler
void Default_IRQHandler(void)
{

}

/**
  * @}
  */
/**
  * @}
  */

