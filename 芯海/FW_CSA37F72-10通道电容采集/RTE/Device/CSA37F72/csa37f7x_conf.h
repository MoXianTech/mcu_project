/**
  * @file    csa37f7x_conf.h 
  * @brief   Library configuration file.
  * @author  ChipSea MCU Group
  * @version V1.0.0
  * @date   2018.11.01         
  * @copyright CHIPSEA TECHNOLOGIES (SHENZHEN) CORP.
  * @note 
  * <h2><center>&copy; COPYRIGHT 2021 ChipSea</center></h2>
  *
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CSA37F7X_CONF_H__
#define __CSA37F7X_CONF_H__

#include "RTE_Components.h"             /* Component selection */

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* ########################## Module Selection ############################## */
/**
  * @brief This is the list of modules to be used in the HAL driver 
  */
#ifdef RTE_DEVICE_HAL_COMMON
#define HAL_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_AFE
#define HAL_AFE_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_AFE2
#define HAL_AFE2_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_CRC
#define HAL_CRC_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_FLASH
#define HAL_FLASH_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_GPIO
#define HAL_GPIO_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_I2C
#define HAL_I2C_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_IWDT
#define HAL_IWDT_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_PWR
#define HAL_PWR_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_RCC
#define HAL_RCC_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_TIM
#define HAL_TIM_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_TIM1
#define HAL_TIM1_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_UART
#define HAL_UART_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_USART
#define HAL_USART_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_WWDT
#define HAL_WWDT_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_SYSCFG
#define HAL_SYSCFG_MODULE_ENABLED
#endif
#ifdef RTE_DEVICE_HAL_FRAMEWORK
#define HAL_FRAMEWORK_MODULE_ENABLED
#endif

/* ########################### System Configuration ######################### */
/**
  * @brief This is the HAL system configuration section
  */     
#define  VDD_VALUE                    3300U  /*!< Value of VDD in mv */           
#define  TICK_INT_PRIORITY            ((uint32_t)(1U<<__NVIC_PRIO_BITS) - 1U) /*!< tick interrupt priority (lowest by default)             */
                                                                              /*  Warning: Must be set to higher priority for HAL_Delay()  */
                                                                              /*  and HAL_GetTick() usage under interrupt context          */
#define  USE_RTOS                     0U
#define  PREFETCH_ENABLE              1U
#define  INSTRUCTION_CACHE_ENABLE     0U
#define  DATA_CACHE_ENABLE            0U
#define  USE_SPI_CRC                  1U

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/*#define USE_FULL_ASSERT    1*/

/* Includes ------------------------------------------------------------------*/
/**
  * @brief Include module's header file 
  */

#ifdef HAL_RCC_MODULE_ENABLED
    #include "csa37f7x_rcc.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED
    #include "csa37f7x_gpio.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_AFE_MODULE_ENABLED
    #include "csa37f7x_afe.h"
#endif /* HAL_AFE_MODULE_ENABLED */

#ifdef HAL_AFE2_MODULE_ENABLED
    #include "csa37f7x_afe2.h"
#endif /* HAL_AFE2_MODULE_ENABLED */

#ifdef HAL_CRC_MODULE_ENABLED
    #include "csa37f7x_crc.h"
#endif /* HAL_CRC_MODULE_ENABLED */

#ifdef HAL_FLASH_MODULE_ENABLED
    #include "csa37f7x_flash.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
    #include "csa37f7x_i2c.h"
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_IWDT_MODULE_ENABLED
    #include "csa37f7x_iwdt.h"
#endif /* HAL_IWDT_MODULE_ENABLED */

#ifdef HAL_PWR_MODULE_ENABLED
    #include "csa37f7x_pwr.h"
#endif /* HAL_PWR_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
    #include "csa37f7x_tim.h"
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_TIM1_MODULE_ENABLED
    #include "csa37f7x_tim1.h"
#endif /* HAL_TIM1_MODULE_ENABLED */

#ifdef HAL_UART_MODULE_ENABLED
    #include "csa37f7x_uart.h"
#endif /* HAL_UART_MODULE_ENABLED */

#ifdef HAL_USART_MODULE_ENABLED
    #include "csa37f7x_usart.h"
#endif /* HAL_USART_MODULE_ENABLED */

#ifdef HAL_WWDT_MODULE_ENABLED
    #include "csa37f7x_wwdt.h"
#endif /* HAL_WWDT_MODULE_ENABLED */

#ifdef HAL_SYSCFG_MODULE_ENABLED
    #include "csa37f7x_syscfg.h"
#endif /* HAL_SYSCFG_MODULE_ENABLED */

#ifdef HAL_FRAMEWORK_MODULE_ENABLED
    #include "csa37f7x_misc.h"
#endif /* HAL_FRAMEWORK_MODULE_ENABLED */
/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed. 
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((char *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
    void assert_failed(char* file, uint32_t line);
#else
    #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */    
    
#ifdef __cplusplus
}
#endif

#endif /* __CSA37F7X_CONF_H__ */

