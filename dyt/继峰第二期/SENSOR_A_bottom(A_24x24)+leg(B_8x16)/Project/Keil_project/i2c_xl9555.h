#ifndef __I2C_XL9555_H__
#define __I2C_XL9555_H__

int8_t xl9555_init(void);
void xl9555_io_set(uint8_t io_num, uint8_t value);
void single_xl9555_io_set(uint8_t io_num);

#endif


