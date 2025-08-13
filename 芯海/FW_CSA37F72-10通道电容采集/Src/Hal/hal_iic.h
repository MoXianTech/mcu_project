/**
* @file                hal_iic.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_IIC_H
#define HAL_IIC_H

#include <stdint.h>                   /* Include standard types */

#define I2CSLAVE_ADDR (0xA0)        //slave address
#define BIT(X)		((uint32_t)(1)<<(X))

#define BUF_LEN					32
#define I2C_TIMING              0xF056050D
#define I2C_SLAVE				1
#define I2C_FLAG_TIMEOUT        ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT        ((uint32_t)(10 * I2C_FLAG_TIMEOUT))

#define I2C_MASTER_ADDR			0xE4		//从设备地址

void hal_iic_init(void);
void hal_iic_clear_busy_count(void);
unsigned char hal_iic_busy_error(void);
uint8_t hal_i2c_reg_write(uint8_t reg, uint8_t value);
uint8_t hal_i2c_reg_read(uint8_t reg, uint8_t *value);
uint8_t i2c_master_write(uint32_t addr, uint8_t* pbuffer,  uint8_t num);
uint8_t i2c_master_read(uint32_t addr, uint8_t* pbuffer,  uint8_t num);

#endif /* HAL_IIC_H */


/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
