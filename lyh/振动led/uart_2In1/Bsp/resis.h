#ifndef  __RESIS_h
#define __RESIS_h
#include <stdint.h>

uint32_t return_resis(void);
void mix_date_init(void);
void transmit_mix_data(int16_t pressure,uint32_t resis);

#pragma pack(1) 
union MIX_pressure_resis_Command_t {
    uint8_t data[10]; // 用于存储整个命令的字节数组
    struct  {
        uint8_t Head1;         	 //首地址1
        uint8_t Head2;           //首地址2
        uint8_t cmd;         		 //校验功能码
        int16_t pressure;   		 // 压力
        uint32_t resis;          // 电阻
        uint8_t tail;            // 尾字节
    } MIXCmd;
};
#pragma pack() 


#endif
