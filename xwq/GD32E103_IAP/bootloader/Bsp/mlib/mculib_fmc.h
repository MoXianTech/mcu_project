//UTF-8
#ifndef _MCULIB_FMC_H_
#define _MCULIB_FMC_H_


#include "gd32e10x.h"
#include "gd32e10x_fmc.h"



//512byte
#define MCULIB_FMC_PAGE_SIZE               ((uint16_t)0x400U)
//1K -2k
#define MCULIB_FMC_WRITE_START_ADDR        ((uint32_t)0x08000400U)
#define MCULIB_FMC_WRITE_END_ADDR          ((uint32_t)0x08000800U)

//在整个eeprom扇区中，实际用到的数量，必须为4的倍数
//这个长度会同步创建一个相同长度的RAM区域
#define MCULIB_FMC_RAM_SIZE                 20



uint8_t mculib_fmc_ram_write_byte(uint16_t sit,uint8_t value);
uint8_t mculib_fmc_ram_write_bytes(uint8_t *value,uint16_t start,uint16_t len);

uint8_t *mculib_fmc_eeprom_get_pt(void);

void mculib_fmc_program(void);
uint8_t mculib_fmc_timer_program(uint16_t div_ms,uint16_t savetime);

uint8_t mculib_fmc_init(void);


#endif
