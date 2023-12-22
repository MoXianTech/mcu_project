//UTF-8
#include "mculib_fmc.h"

//绝对定位
const char mculib_fmc_eeprom [MCULIB_FMC_WRITE_END_ADDR-MCULIB_FMC_WRITE_START_ADDR] __attribute__((at(MCULIB_FMC_WRITE_START_ADDR)));
//必须4或8字节对齐
__attribute__((aligned(8))) uint8_t mculib_fmc_ram[MCULIB_FMC_RAM_SIZE];

enum
{
    MCULIB_FMC_RAM_CHANGE_YES=0,
    MCULIB_FMC_RAM_CHANGE_NO
}mculib_fmc_ram_change_flag=MCULIB_FMC_RAM_CHANGE_NO;

/*
批量进行页擦除
起始地址：MCULIB_FMC_WRITE_START_ADDR
结束地址：MCULIB_FMC_WRITE_END_ADDR
*/
void mculib_fmc_erase_pages(void)
{
    uint32_t EraseCounter;
    uint32_t PageNum = (MCULIB_FMC_WRITE_END_ADDR - MCULIB_FMC_WRITE_START_ADDR) / MCULIB_FMC_PAGE_SIZE;
    fmc_unlock();
    fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
    for(EraseCounter = 0U; EraseCounter < PageNum; EraseCounter++){
        fmc_page_erase(MCULIB_FMC_WRITE_START_ADDR + (MCULIB_FMC_PAGE_SIZE * EraseCounter));
        fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
    }
    fmc_lock();
}

void mculib_fmc_program_base(void)
{
    uint32_t address = MCULIB_FMC_WRITE_START_ADDR;
    uint32_t *add;
    uint8_t i=0;

    add=(uint32_t *)mculib_fmc_ram;
    
    fmc_unlock();
    while(address+4 < MCULIB_FMC_WRITE_END_ADDR)
    {
        fmc_word_program(address, *add);
        address += 4U;
        add+=1U;
        i+=4;
        fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
        if(i>=MCULIB_FMC_RAM_SIZE)
        {
            break;
        }
    }
    fmc_lock();
}

uint8_t mculib_fmc_ram_write_byte(uint16_t sit,uint8_t value)
{
    if(sit>=MCULIB_FMC_RAM_SIZE)return 1;
    mculib_fmc_ram[sit]= value;
    mculib_fmc_ram_change_flag=MCULIB_FMC_RAM_CHANGE_YES;
    return 0;
}



uint8_t mculib_fmc_ram_write_bytes(uint8_t *value,uint16_t start,uint16_t len)
{
    uint16_t i;
    for(i=start;i<len;i++)
    {
        if(i>=MCULIB_FMC_RAM_SIZE)
        {
            return 1;
        }
        else 
        {
            mculib_fmc_ram[i]= value[i-start];
        }
    }
    mculib_fmc_ram_change_flag=MCULIB_FMC_RAM_CHANGE_YES;
    return 0;
}
void mculib_fmc_ram_sum_not(void)
{
    mculib_fmc_ram[MCULIB_FMC_RAM_SIZE-1]=0;
    for(uint16_t i=0;i<MCULIB_FMC_RAM_SIZE-2;i++)
    {
        mculib_fmc_ram[MCULIB_FMC_RAM_SIZE-1]+=mculib_fmc_ram[i];
    }
    mculib_fmc_ram[MCULIB_FMC_RAM_SIZE-2]=~mculib_fmc_ram[MCULIB_FMC_RAM_SIZE-1];
}

uint8_t *mculib_fmc_eeprom_get_pt(void)
{
    return (uint8_t *)(MCULIB_FMC_WRITE_START_ADDR);
}

 
void mculib_fmc_program(void)
{
    mculib_fmc_ram_change_flag=MCULIB_FMC_RAM_CHANGE_NO;
    mculib_fmc_erase_pages();
    mculib_fmc_ram_sum_not();
    mculib_fmc_program_base();
}
uint8_t mculib_fmc_timer_program(uint16_t div_ms,uint16_t savetime)
{
    static uint16_t time_ms=0;
    if(mculib_fmc_ram_change_flag==MCULIB_FMC_RAM_CHANGE_YES)time_ms += div_ms;
    else time_ms=0;
    if(time_ms>=savetime)
    {
        time_ms=0;
        mculib_fmc_ram_change_flag=MCULIB_FMC_RAM_CHANGE_NO;
        mculib_fmc_erase_pages();
        mculib_fmc_ram_sum_not();
        mculib_fmc_program_base();
        return 0;
    }
    return 1;
}
uint8_t mculib_fmc_init(void)
{
    uint8_t i=0,sum=0;
    for(i=0;i<MCULIB_FMC_RAM_SIZE-2;i++)
    {
        sum+=mculib_fmc_eeprom[i];
    }
    if(mculib_fmc_eeprom[MCULIB_FMC_RAM_SIZE-1]+mculib_fmc_eeprom[MCULIB_FMC_RAM_SIZE-2]!=255)return 1;
    if(sum==mculib_fmc_eeprom[MCULIB_FMC_RAM_SIZE-1])
    {
        for(i=0;i<MCULIB_FMC_RAM_SIZE;i++)
        {
            mculib_fmc_ram[i]=mculib_fmc_eeprom[i];
        }
        return 0;
    }
    return 1;
}

