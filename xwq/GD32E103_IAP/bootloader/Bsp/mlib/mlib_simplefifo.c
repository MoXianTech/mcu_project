//UTF-8
#include "mlib_simplefifo.h"

SIMPLEFIFO_TYPE simplefifo_buff[SIMPLEFIFO_BUFF_SIZE];
uint16_t    simplefifo_buff_write_=0;
uint16_t    simplefifo_buff_read_=0;
uint8_t    simplefifo_buff_overflag_=0;


SIMPLEFIFO_ERROR simplefifo_buff_write(SIMPLEFIFO_TYPE value)
{
    if(simplefifo_buff_write_!=simplefifo_buff_read_||simplefifo_buff_overflag_==0)
    {
        simplefifo_buff[simplefifo_buff_write_]=value;
        simplefifo_buff_write_++;
        if(simplefifo_buff_write_>=SIMPLEFIFO_BUFF_SIZE)
        {
            simplefifo_buff_write_=0;
            simplefifo_buff_overflag_=1;
        }
        return SIMPLEFIFO_ERROR_NONE;
    }
    return SIMPLEFIFO_ERROR_BUFF_FULL;
}
uint16_t  simplefifo_buff_cleare_all(void)
{
    simplefifo_buff_overflag_=0;
    simplefifo_buff_read_=0;
    simplefifo_buff_write_=0;
		return 0;
}
//数据读取，判断是否还有新数据未读
SIMPLEFIFO_ERROR simplefifo_buff_pt_read(SIMPLEFIFO_TYPE *value)
{
    if(simplefifo_buff_write_!=simplefifo_buff_read_||simplefifo_buff_overflag_==1)
    {
        *value=simplefifo_buff[simplefifo_buff_read_];
        simplefifo_buff_read_++;
        if(simplefifo_buff_read_>=SIMPLEFIFO_BUFF_SIZE)
        {
            simplefifo_buff_read_=0;
            simplefifo_buff_overflag_=0;
        }
        return SIMPLEFIFO_ERROR_NONE; 
    }
    return SIMPLEFIFO_ERROR_BUFF_NULL;  
}

SIMPLEFIFO_TYPE simplefifo_buff_read(void)
{
    SIMPLEFIFO_TYPE value;
    if(simplefifo_buff_write_!=simplefifo_buff_read_||simplefifo_buff_overflag_==1)
    {
        value=simplefifo_buff[simplefifo_buff_read_];
        simplefifo_buff_read_++;
        if(simplefifo_buff_read_>=SIMPLEFIFO_BUFF_SIZE)
        {
            simplefifo_buff_read_=0;
            simplefifo_buff_overflag_=0;
        }
        return value;
    }
    return 0;
}

uint16_t  simplefifo_buff_length_get(void)
{
    if(simplefifo_buff_overflag_)
    {
        return simplefifo_buff_read_-simplefifo_buff_write_+SIMPLEFIFO_BUFF_SIZE;
    }
    else 
    {
        return simplefifo_buff_write_-simplefifo_buff_read_;
    }
}



