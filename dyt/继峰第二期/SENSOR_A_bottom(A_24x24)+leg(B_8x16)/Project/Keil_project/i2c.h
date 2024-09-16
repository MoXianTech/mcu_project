#ifndef __I2C_H__
#define __I2C_H__
#include "gd32e10x.h"



#define I2C0_OWN_ADDRESS7     0x72

#define I2C0_SLAVE_0_ADDRESS7 0x40
#define I2C0_SLAVE_1_ADDRESS7 0x42
#define I2C0_SLAVE_2_ADDRESS7 0x44


typedef enum I2C_COM {
    I2C_COM0,
    I2C_COM1
} I2C_COM_T;

void i2c_init(I2C_COM_T com, uint32_t speed);
void i2c_thread_task(void);
uint8_t i2c_read_reg(I2C_COM_T com, uint8_t address, uint8_t reg);
int i2c_write_data(I2C_COM_T com, uint8_t address, uint8_t reg, uint8_t value);

#endif

