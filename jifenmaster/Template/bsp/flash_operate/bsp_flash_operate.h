#ifndef __FLASH_OPERATE_H__
#define __FLASH_OPERATE_H__
#include "stdint.h"

/* write 8 bit length data to a given address */
void fmc_write_8bit_data(uint32_t address, uint16_t length, int8_t* data_8);
/* read 8 bit length data from a given address */
void fmc_read_8bit_data(uint32_t address, uint16_t length, int8_t* data_8);

#endif


