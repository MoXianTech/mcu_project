/**
* @file                hal_soft_iic.c
* @brief
* @author
* @version             V0.1.0
* @date                2021.8.9
* @details
* @copyright           (C) COPYRIGHT 2021 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef HAL_SOFT_IIC_H
#define HAL_SOFT_IIC_H

#include <stdint.h>                   /* Include standard types */

#define SOFT_IIC_SLAVE_ADDR			0xB4		//从设备地址
#define SOFT_IIC_REGLEN				1


/* I2C的初始化工作（必须调用） */
void iic_soft_init(void);

void SetIIC_Speed(unsigned char speed);

unsigned char GetIIC_Speed(void);

/* 
	slaveAddress :从设备地址 
	regAddress	 :从设备中寄存器的地址
	data				 :要写入的数据
	return       :要读的数据
*/
uint8_t I2C_WriteData(uint8_t slaveAddress , uint16_t regAddress ,uint8_t regLen ,uint8_t * data, uint16_t len);
unsigned char hal_i2c_write_soft(unsigned char slaveAddress, unsigned int regAddress ,unsigned char regLen ,unsigned char *data, unsigned int len);



/* 
	slaveAddress :从设备地址 
	regAddress	 :从设备中寄存器的地址
	return       :要读的数据
*/
uint8_t I2C_ReadData(uint8_t slaveAddress ,uint16_t regAddress,uint8_t regLen , uint8_t* pBuffer,uint16_t len);
unsigned char hal_i2c_read_soft(unsigned char slaveAddress ,unsigned int regAddress,unsigned char regLen , unsigned char* pBuffer,unsigned int len);



#endif /* HAL_SOFT_IIC_H */