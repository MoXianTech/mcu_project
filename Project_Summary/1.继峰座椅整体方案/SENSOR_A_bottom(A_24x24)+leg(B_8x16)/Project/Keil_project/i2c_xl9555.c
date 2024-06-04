#include "gd32e10x.h"
#include "i2c.h"
#include "i2c_XL9555.h"


uint8_t xl9555_rank[3] = {I2C0_SLAVE_0_ADDRESS7, I2C0_SLAVE_1_ADDRESS7, I2C0_SLAVE_2_ADDRESS7};

#define START_CONFIG_REG 0x06  //config Port0
#define END_CONFIG_REG 0x07    //config Port1

#define START_IO_REG 0x02      //output Port 0
#define END_IO_REG 0x03        //output Port 1

int8_t xl9555_init(void)
{
    uint8_t count = 0;

    for (count = 0; count < 3; count ++)
    {
        i2c_write_data(I2C_COM0, xl9555_rank[count], START_CONFIG_REG, 0x00);
        i2c_write_data(I2C_COM0, xl9555_rank[count], END_CONFIG_REG, 0x00);

        i2c_write_data(I2C_COM0, xl9555_rank[count], START_IO_REG, 0x00);
        i2c_write_data(I2C_COM0, xl9555_rank[count], END_IO_REG, 0x00);

    }

    return 0;
}

void single_xl9555_io_set(uint8_t io_num)
{
		if (io_num < 24)
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 8], START_IO_REG, 0x80 >> (io_num % 8));
    else
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 8 - 3], END_IO_REG, 0x01 << (io_num % 8));
}

inline void total_xl9555_io_clean(uint8_t io_num)
{
    if (io_num < 24)
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 8], START_IO_REG, 0x00);
    else
        i2c_write_data(I2C_COM0, xl9555_rank[io_num / 8 - 3], END_IO_REG, 0x00);
}

void xl9555_io_set(uint8_t io_num, uint8_t value)
{
    static uint8_t gate_num = 0;

    if (io_num >= 48)
        return ;

    if (gate_num / 8 != io_num / 8)
    {
        total_xl9555_io_clean(gate_num);
    }

    single_xl9555_io_set(io_num);

    gate_num = io_num;
 
}

