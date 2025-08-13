/**
* @file                hal_gpio.c
* @brief
* @author
* @version             V0.1.0
* @date                2021.6.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup HAL层
  * @{
  */
/**
 * @defgroup GPIO驱动
 * @brief GPIO驱动
 * @details 
 * @{
 */
//Driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_misc.h"
//Hal
#include "hal_gpio.h"

/**
* @fn void hal_gpio_init(void)
* @brief gpio init
* @param None
* @return None.
*/

void hal_gpio_init(void)            // 通用GPIO初始化
{
    gpio_config_t ptr_config;

    rcc_ahb_periph_clock_enable_ctrl(RCC_AHBPeriph_GPIOA, ENABLE);              //使能GPIOA时钟
    rcc_ahb_periph_clock_enable_ctrl(RCC_AHBPeriph_GPIOB, ENABLE);              //使能GPIOB时钟

    ptr_config.mode = GPIO_MODE_IN;
    ptr_config.pin = GPIO_PIN_0 /*| GPIO_PIN_1 */|GPIO_PIN_2| GPIO_PIN_3 /*| GPIO_PIN_10 | GPIO_PIN_11*/;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIO_GROUP_A,&ptr_config);

    ptr_config.mode = GPIO_MODE_OUT_OD;
    ptr_config.pin = GPIO_PIN_1/* | GPIO_PIN_3| GPIO_PIN_2*/;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIO_GROUP_A,&ptr_config);

    ptr_config.mode = GPIO_MODE_OUT_PP;
    ptr_config.pin = GPIO_PIN_4 | GPIO_PIN_5;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIO_GROUP_B,&ptr_config);

    //ptr_config.mode = GPIO_MODE_AN;
    //ptr_config.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 /*| GPIO_PIN_10 | GPIO_PIN_11*/;
    //ptr_config.pull = GPIO_PULL_NO_PULL;
    //gpio_init(GPIO_GROUP_A,&ptr_config);
}



/**
* @fn void hal_gpio_int_out_init(void)
* @brief gpio int_out pin init
* @param None
* @return None.
*/
void hal_gpio_int_out_init(void)            // 中断输出gpio初始化
{
    gpio_config_t ptr_config;
#if(HAL_GPIO_INT_OUT != HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_OUT_OD;
    ptr_config.pin =  HAL_GPIO_INT_OUT_PIN;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(HAL_GPIO_INT_OUT_GROUP,&ptr_config);
    gpio_bit_write(HAL_GPIO_INT_OUT_GROUP, HAL_GPIO_INT_OUT, BIT_SET);
#endif
}

/**
* @fn void hal_gpio_key_init(void)
* @brief key gpio init.
* @param None
* @return None.
*/
void hal_gpio_key_init(void)                // 按键电平输出GPIO初始化
{
    gpio_config_t ptr_config;
    // key 0
#if(HAL_GPIO_KEY0!=HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_OUT_OD;
	ptr_config.pin =  HAL_GPIO_KEY0_PIN;
	ptr_config.pull = GPIO_PULL_NO_PULL;
	gpio_init(HAL_GPIO_KEY0_GROUP,&ptr_config);
    gpio_bit_write(HAL_GPIO_KEY0_GROUP, HAL_GPIO_KEY0, BIT_SET);
#endif
    // key 1
#if(HAL_GPIO_KEY1!=HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_OUT_OD;
	ptr_config.pin =  HAL_GPIO_KEY1_PIN;
	ptr_config.pull = GPIO_PULL_NO_PULL;
	gpio_init(HAL_GPIO_KEY1_GROUP,&ptr_config);
    gpio_bit_write(HAL_GPIO_KEY1_GROUP, HAL_GPIO_KEY1, BIT_SET);
#endif
    // key 2
#if(HAL_GPIO_KEY2!=HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_OUT_OD;
	ptr_config.pin =  HAL_GPIO_KEY2_PIN;
	ptr_config.pull = GPIO_PULL_NO_PULL;
	gpio_init(HAL_GPIO_KEY2_GROUP,&ptr_config);
    gpio_bit_write(HAL_GPIO_KEY2_GROUP, HAL_GPIO_KEY2, BIT_SET);
#endif
    // key 3
#if(HAL_GPIO_KEY3!=HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_OUT_OD;
	ptr_config.pin =  HAL_GPIO_KEY3_PIN;
	ptr_config.pull = GPIO_PULL_NO_PULL;
	gpio_init(HAL_GPIO_KEY3_GROUP,&ptr_config);
    gpio_bit_write(HAL_GPIO_KEY3_GROUP, HAL_GPIO_KEY3, BIT_SET);
#endif
}

/**
* @fn void hal_gpio_int_out_high(void)
* @brief gpio int_out pin set high
* @param None
* @return None.
*/
void hal_gpio_int_out_high(void)            //设置中断输出GPIO为高电平
{
#if(HAL_GPIO_INT_OUT != HAL_GPIO_PINx_NONE)
    gpio_bit_write(HAL_GPIO_INT_OUT_GROUP, HAL_GPIO_INT_OUT, BIT_SET);
#endif
}

/**
* @fn void hal_gpio_int_out_low(void)
* @brief gpio int_out pin set low
* @param None
* @return None.
*/
void hal_gpio_int_out_low(void)            //设置中断输出GPIO为低电平
{
#if(HAL_GPIO_INT_OUT != HAL_GPIO_PINx_NONE)
    gpio_bit_write(HAL_GPIO_INT_OUT_GROUP, HAL_GPIO_INT_OUT, BIT_RESET);
#endif
}

/**
* @fn void hal_gpio_set_key_pin(uint16_t gpio, uint8_t status)
* @brief gpio key pin set high or low
* @param [in]gpio: gpio num, [in]status: high or low
* @return None.
*/
void hal_gpio_set_key_pin(uint16_t gpio, uint8_t status)    // 按键输出GPIO电平设置
{
    switch(gpio)
    {
        #if(HAL_GPIO_KEY0!=HAL_GPIO_PINx_NONE)
        case HAL_GPIO_KEY0:
            gpio_bit_write(HAL_GPIO_KEY0_GROUP, HAL_GPIO_KEY0, status);
            break;
        #endif

        #if(HAL_GPIO_KEY1!=HAL_GPIO_PINx_NONE)
        case HAL_GPIO_KEY1:
            gpio_bit_write(HAL_GPIO_KEY1_GROUP, HAL_GPIO_KEY1, status);
            break;
        #endif

        #if(HAL_GPIO_KEY2!=HAL_GPIO_PINx_NONE)
        case HAL_GPIO_KEY2:
            gpio_bit_write(HAL_GPIO_KEY2_GROUP, HAL_GPIO_KEY2, status);
            break;
        #endif

        #if(HAL_GPIO_KEY3!=HAL_GPIO_PINx_NONE)
        case HAL_GPIO_KEY3:
            gpio_bit_write(HAL_GPIO_KEY3_GROUP, HAL_GPIO_KEY3, status);
            break;
        #endif

        default: break;
    }
}

/**
* @fn void hal_gpio_int_in_init(void)
* @brief gpio int_in init
* @param None
* @return None.
*/

void hal_gpio_int_in_init(void)                             // 中断输入gpio初始化
{
    nvic_config_t nvic_config;
    gpio_config_t ptr_config;
    
#if(HAL_GPIO_INT_IN!=HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_IN;
    ptr_config.pin =  HAL_GPIO_INT_IN_PIN;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(HAL_GPIO_INT_IN_GROUP,&ptr_config);
    gpio_interrupt_config(HAL_GPIO_INT_IN_GROUP, HAL_GPIO_INT_IN_PIN, GPIO_IT_FALLING);     // 设置下降沿模式
    //gpio_interrupt_config(HAL_GPIO_INT_IN_GROUP, HAL_GPIO_INT_IN, GPIO_IT_LOW);
#if(HAL_GPIO_INT_IN_GROUP == 0)                // 使能PA输入口中断功能
    nvic_config.nvic_channel_priority = 1;
    nvic_config.nvic_enable_flag = ENABLE;
    nvic_config.nvic_IRQ_channel = IRQn_GPIOA;
    nvic_init(&nvic_config);
#endif

#if(HAL_GPIO_INT_IN_GROUP == 1)                // 使能PB输入口中断功能
    nvic_config.nvic_channel_priority = 1;
    nvic_config.nvic_enable_flag = ENABLE;
    nvic_config.nvic_IRQ_channel = IRQn_GPIOB;
    nvic_init(&nvic_config);
#endif
#endif
}


/**
* @fn uint8_t hal_gpio_get_int_in_pin(void)
* @brief get int_in pin stauts
* @param None
* @return 0:low , 1: high
*/
uint8_t hal_gpio_get_int_in_pin(void)                       // 获取中断输入GPIO电平状态
{
#if(HAL_GPIO_INT_IN!=HAL_GPIO_PINx_NONE)
    return gpio_input_data_bit_read(HAL_GPIO_INT_IN_GROUP,HAL_GPIO_INT_IN_PIN);
#endif
}

/**
* @fn uint8_t hal_gpio_get_int_in_int_state_claer(void)
* @brief get int_in pin stauts
* @param None
* @return 0:low , 1: high
*/
void hal_gpio_get_int_in_int_state_clear(void)              // »ñÈ¡ÖÐ¶ÏÊäÈëGPIOµçÆ½×´Ì¬
{
#if(HAL_GPIO_INT_IN!=HAL_GPIO_PINx_NONE)
    gpio_interrupt_flag_clear(HAL_GPIO_INT_IN_GROUP,HAL_GPIO_INT_IN_PIN);
#endif
}

/**
* @fn void hal_gpio_motor_init(void)
* @brief gpio motor pin init
* @param None
* @return None.
*/
void hal_gpio_motor_init(void)                              // 中断输出gpio初始化
{
    gpio_config_t ptr_config;
    
#if(HAL_GPIO_MOTOR0 != HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_OUT_OD;
    ptr_config.pin =  HAL_GPIO_MOTOR0;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(HAL_GPIO_MOTOR0_GROUP,&ptr_config);
    gpio_bit_write(HAL_GPIO_MOTOR0_GROUP, HAL_GPIO_MOTOR0, BIT_RESET);
#endif

#if(HAL_GPIO_MOTOR1 != HAL_GPIO_PINx_NONE)
    ptr_config.mode = GPIO_MODE_OUT_OD;
    ptr_config.pin =  HAL_GPIO_MOTOR1;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(HAL_GPIO_MOTOR1_GROUP,&ptr_config);
    gpio_bit_write(HAL_GPIO_MOTOR1_GROUP, HAL_GPIO_MOTOR1, BIT_RESET);
#endif
}

/**
* @fn void hal_gpio_set_motor0_high(void)
* @brief gpio motor0 pin set high
* @param None
* @return None.
*/
void hal_gpio_set_motor0_high(void)            //设置motor0为高电平
{
#if(HAL_GPIO_MOTOR0 != HAL_GPIO_PINx_NONE)
    gpio_bit_write(HAL_GPIO_MOTOR0_GROUP, HAL_GPIO_MOTOR0, BIT_SET);
#endif
}

/**
* @fn void hal_gpio_set_motor0_low(void)
* @brief gpio motor0 pin set low
* @param None
* @return None.
*/
void hal_gpio_set_motor0_low(void)            //设置motor0为低电平
{
#if(HAL_GPIO_MOTOR0 != HAL_GPIO_PINx_NONE)
    gpio_bit_write(HAL_GPIO_MOTOR0_GROUP, HAL_GPIO_MOTOR0, BIT_RESET);
#endif
}
/**
* @fn void hal_gpio_set_motor1_high(void)
* @brief gpio motor1 pin set high
* @param None
* @return None.
*/
void hal_gpio_set_motor1_high(void)            //设置motor1为高电平
{
#if(HAL_GPIO_MOTOR1 != HAL_GPIO_PINx_NONE)
    gpio_bit_write(HAL_GPIO_MOTOR1_GROUP, HAL_GPIO_MOTOR1, BIT_SET);
#endif
}

/**
* @fn void hal_gpio_set_motor1_low(void)
* @brief gpio motor1 pin set low
* @param None
* @return None.
*/
void hal_gpio_set_motor1_low(void)            //设置motor1为低电平
{
#if(HAL_GPIO_MOTOR1 != HAL_GPIO_PINx_NONE)
    gpio_bit_write(HAL_GPIO_MOTOR1_GROUP, HAL_GPIO_MOTOR1, BIT_RESET);
#endif
}

/**
  * @}
  */

/**
  * @}
  */
