/**
* @file                hal_iic.c
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
 * @defgroup IIC驱动
 * @brief IIC驱动
 * @details 
 * @{
 */
//driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_i2c.h"
#include "csa37f7x_syscfg.h"
#include "csa37f7x_misc.h"

//hal
#include "hal_iic.h"

static volatile char iicErrTime = 0;

static uint8_t rx_buffer[BUF_LEN] = {0};
static uint8_t tx_buffer[BUF_LEN] = {0};
static uint32_t timeout = I2C_LONG_TIMEOUT;
static uint32_t timing_delay = 0;

/**
* @fn void hal_iic_init(void)
* @brief Initial iic hardware.
* @param None
* @return None.
*/
void hal_iic_init(void)                                     // IIC模块初始化
{
    gpio_config_t ptr_config;
    nvic_config_t nvic_config;
    /*定义I2C配置结构体 */
    i2c_config_t i2c_initstruct;

   /*打开iic 时钟  */
    rcc_apb_periph_clock_enable_ctrl(RCC_APBPeriph_I2C, ENABLE);
    /*复用PA10为SCL */
    gpio_mf_config(GPIO_GROUP_A,GPIO_PIN_NUM10,GPIO_MUX_FUNC_2);
    /*复用PA11为SDA */
    gpio_mf_config(GPIO_GROUP_A,GPIO_PIN_NUM11,GPIO_MUX_FUNC_2);

    /*使能GPIOA时钟*/
    rcc_ahb_periph_clock_enable_ctrl(RCC_AHBPeriph_GPIOA, ENABLE);
    /*PA10/PA11设置为输入模式，不上拉*/
    ptr_config.mode = GPIO_MODE_IN;
    ptr_config.pin = GPIO_PIN_10|GPIO_PIN_11;
    ptr_config.pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIO_GROUP_A,&ptr_config);
//    gpio_interrupt_config(GPIO_GROUP_A,GPIO_PIN_NUM11,GPIO_IT_FALLING);

#if I2C_SLAVE
    /*使能发送，接收，地址匹配，stop检测中断 */
    i2c_interrupt_config((I2C_CTR1_TXISIE|I2C_CTR1_RXNEIE|I2C_CTR1_ADRMIE|I2C_CTR1_STOPDIE),ENABLE); 
    /*时钟延展使能 */
    i2c_stretch_clock_enable_ctrl(ENABLE);
    /*指定I2C_TMR寄存器值（需要参考I2C时序） */
    i2c_initstruct.tmr_value = 0x00310309;
    /*关闭模拟噪声滤波 */
    i2c_initstruct.analog_flag = I2C_CTR1_AFDIS;
    /*关闭数字噪声滤波 */
    i2c_initstruct.digital_value = 0x00;
    /*设置I2C总线模式 */
    i2c_initstruct.mode = I2C_BUS_MODE_I2C;
    /*设置I2C从机地址 */
    i2c_initstruct.slave_addr1 = 0xA0;
    /*设置应答使能 */
    i2c_initstruct.ack_flag = I2C_ACK_ENABLE;
    /*设置应答地址 */
    i2c_initstruct.ack_addr = I2C_ACK_ADDR_7BITS;
    /*初始化I2C */
    i2c_init(&i2c_initstruct);
    /*设置I2C中断唤醒 */
    i2c_wakeup_enable_ctrl(ENABLE);
    /*启动I2C */
    i2c_enable_ctrl(ENABLE);

    /*设置I2C中断优先级为0,使能I2C中断 */
    nvic_config.nvic_channel_priority = 0x00;
    nvic_config.nvic_enable_flag = ENABLE;
    nvic_config.nvic_IRQ_channel = IRQn_I2C;
    nvic_init(&nvic_config);
#else
    /* I2C configuration */
    i2c_initstruct.mode = I2C_BUS_MODE_I2C;
    i2c_initstruct.analog_flag = I2C_ANALOG_FILTER_ENABLE;
    i2c_initstruct.digital_value = 0x00;
    i2c_initstruct.slave_addr1 = 0x00;
    i2c_initstruct.ack_flag = I2C_ACK_ENABLE;
    i2c_initstruct.ack_addr = I2C_ACK_ADDR_7BITS;
    i2c_initstruct.tmr_value = I2C_TIMING;   
  
    i2c_init(&i2c_initstruct);
    i2c_enable_ctrl(ENABLE);
#endif
	
}

/**
* @fn unsigned char hal_iic_busy_error(void)
* @brief Patch for iic bus timeout
* @param None
* @return None.
*/
void hal_iic_clear_busy_count(void)
{
    iicErrTime = 0;
}


/**
* @fn unsigned char hal_iic_busy_error(void)
* @brief Patch for iic bus timeout
* @param None
* @return None.
*/
unsigned char hal_iic_busy_error(void)                      // 检测IIC是否异常忙
{
    if(I2C->STS&BIT(15))
    {
        iicErrTime++;
    }

    if(iicErrTime > 2)                                      // Ã¦×´Ì¬³¬Ê±£¬ÔòÖØÐÂ³õÊ¼»¯IIC
    {

        hal_iic_init();
        iicErrTime = 0;
        return 1;
    }

    return 0;
}

uint8_t hal_i2c_reg_write(uint8_t reg, uint8_t value)
{
	uint8_t ret = 0;
	
	tx_buffer[0] = reg;
	tx_buffer[1] = value;
	
	ret = i2c_master_write(I2C_MASTER_ADDR, tx_buffer, 2);
	
	return ret;	
}

uint8_t hal_i2c_reg_read(uint8_t reg, uint8_t *value)
{
	uint8_t ret = 0;

	tx_buffer[0] = reg;
	
	ret = i2c_master_write(I2C_MASTER_ADDR, tx_buffer, 1);
	ret = i2c_master_read(I2C_MASTER_ADDR, value, 1);
	
	return ret;
}

/**
  * @fn     i2c_master_write(uint8_t* pbuffer, uint8_t* num, uint32_t addr )
  * @brief  write a block of data from the salve driver.
  * @param  pbuffer: pointer to the buffer that send the data read from
  *         the salve driver.
  * @param  num_byte_to_read: pointer to the variable holding number of bytes to
  *         be write from the salve driver.
  * @param  addr: specifies the slave address to be programmed.
  * @return sEE_OK (0) if operation is correctly performed, else return value
  *         different from sEE_OK (0) or the I2C_Timeout user callback.
  */

uint8_t i2c_master_write(uint32_t addr, uint8_t* pbuffer, uint8_t num)
{
    uint32_t count = 0;
	
    timeout = I2C_LONG_TIMEOUT;
    while(i2c_flag_status_get(I2C_FLAG_BUSYF) != RESET)
    {
        if((timeout--) == 0)
            return 0;
    }
    i2c_slave_address_config(addr);
    i2c_request_direction_config(I2C_DIRECTION_TX);
    i2c_bytes_number_config(num);
    i2c_start_enable_ctrl(ENABLE);

    while (count != num)
    {
        timeout = I2C_LONG_TIMEOUT;
        while(i2c_flag_status_get(I2C_FLAG_TXEF) == RESET)
        {
            if((timeout--) == 0)
                return 0;
        }
        i2c_data_send((uint8_t)(pbuffer[count]));
        count++;
    }
		
    timeout = I2C_LONG_TIMEOUT;
    while(i2c_flag_status_get(I2C_FLAG_CMPF) == RESET)
    {
        if((timeout--) == 0)
            return 0;
    }
		
    i2c_stop_enable_ctrl(ENABLE);
    while(i2c_flag_status_get(I2C_FLAG_STOPF) == RESET) //Wait until STOPF flag is set
    {
        if((timeout--) == 0)
            return 0;
    }
    i2c_flag_clear(I2C_FLAG_STOPF);

    return 1;
}

/**
  * @fn     i2c_master_read(uint8_t* pbuffer,  uint16_t* num_byte_to_read ,uint32_t addr)
  * @brief  Reads a block of data from the salve driver.
  * @param  pbuffer: pointer to the buffer that receives the data read from
  *         the salve driver.
  * @param  num_byte_to_read: pointer to the variable holding number of bytes to
  *         be read from the salve driver.
  * @param  addr: specifies the slave address to be programmed.
  * @return sEE_OK (0) if operation is correctly performed, else return value
  *         different from sEE_OK (0) or the I2C_Timeout user callback.
  */
uint8_t i2c_master_read(uint32_t addr, uint8_t* pbuffer,  uint8_t num)
{
    uint32_t count;
	
    timeout = I2C_LONG_TIMEOUT;
    while(i2c_flag_status_get(I2C_FLAG_BUSYF) != RESET)
    {
        if((timeout--) == 0)
            return 0;
    }
    i2c_slave_address_config(addr);
    i2c_request_direction_config(I2C_DIRECTION_RX);
    /* Update CR2 : set Slave Address , set read request, generate Start and set end mode */
    i2c_bytes_number_config(num);
    i2c_start_enable_ctrl(ENABLE);
    count = 0;
		
    while (count != num)  //Wait until all data are received
    {
        timeout = I2C_LONG_TIMEOUT;
        while(i2c_flag_status_get(I2C_FLAG_RXNEF) == RESET) //Wait until RXNE flag is set
        {
            if((timeout--) == 0)
                return 0;
        }
        pbuffer[count]= i2c_data_receive();
        count++;
    }

    timeout = I2C_LONG_TIMEOUT;
    while(i2c_flag_status_get(I2C_FLAG_CMPF) == RESET)
    {
        if((timeout--) == 0)
            return 0;
    }
    i2c_stop_enable_ctrl(ENABLE);

    timeout = I2C_LONG_TIMEOUT;
    while(i2c_flag_status_get(I2C_FLAG_STOPF) == RESET)  // Wait until STOPF flag is set
    {
        if((timeout--) == 0)
            return 0;
    }
    i2c_flag_clear(I2C_FLAG_STOPF);
    return 1;
}
/**
  * @}
  */

/**
  * @}
  */
