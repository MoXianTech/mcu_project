/*************************************************************************
	> File Name: i2c_XL9555.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/7/18 17:36:30
 ************************************************************************/


#ifndef __I2C_XL9555_H__
#define __I2C_XL9555_H__

int8_t xl9555_init(void);
void xl9555_io_set(uint8_t io_num, uint8_t value);

#endif

