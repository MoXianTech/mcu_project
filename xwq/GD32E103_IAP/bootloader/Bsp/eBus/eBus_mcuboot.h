//UTF-8

#ifndef _EBUS_MCUBOOT_H_
#define _EBUS_MCUBOOT_H_

#include "eBus_core.h"


#ifndef uint64
#define uint64 unsigned long long
#endif

#ifndef int64
#define int64 signed long long
#endif

#ifndef uint32
#define uint32 unsigned int 
#endif

#ifndef int32
#define int32 signed int 
#endif


#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef int16
#define int16 short
#endif

#ifndef int8
#define int8 char
#endif


//进入BOOT模式
#define EBUS_MCUBOOT_ENABLE                 0x70
#define EBUS_MCUBOOT_ENABLE_ACK             0x90

//
#define EBUS_MCUBOOT_RESET                  0x72
#define EBUS_MCUBOOT_RESET_ACK              0x92


//请求升级
#define EBUS_MCUBOOT_REQUEST                0x74
#define EBUS_MCUBOOT_REQUEST_ACK            0x94

//请求升级
#define EBUS_MCUBOOT_DATA_WRITE             0x78
#define EBUS_MCUBOOT_DATA_WRITE_ACK         0x98

//请求升级
#define EBUS_MCUBOOT_DATA_READ              0x7A
#define EBUS_MCUBOOT_DATA_READ_ACK          0x9A


#define EBUS_MCUBOOT_DATA_PACKERR_ACK       0xC1


typedef enum
{
    EBUS_MCUBOOT_STATE_ENUM_DISABLE=0,
    EBUS_MCUBOOT_STATE_ENUM_ENABLE=1
}EBUS_MCUBOOT_STATE_ENUM;



typedef enum
{
    EBUS_MCUBOOT_ERR_ENUM_NONE=0,
    //文件长度过长
    EBUS_MCUBOOT_ERR_ENUM_OVERLONG=1,
    //文件擦除错误
    EBUS_MCUBOOT_ERR_ENUM_E_ERR=2,
    //
    EBUS_MCUBOOT_ERR_ENUM_ADD_ERR=3,
}EBUS_MCUBOOT_ERR_ENUM;


void eBus_mcuboot_run(
    EBUS_STRUCT *eBus_pack,
    EBUS_FLOW_WRITE_PFUNC write,
    EBUS_MCUBOOT_ERR_ENUM (*rom_erasure)(uint32),//擦除长度
    EBUS_MCUBOOT_ERR_ENUM (*rom_write)(uint32,uint32,uint8 *),//写偏移地址，写长度，写buff指针
    EBUS_MCUBOOT_ERR_ENUM (*rom_read)(uint32,uint32,uint8 *),//偏移地址，读取长度，读buff指针
    void (*delay_ms)(uint32),//延时函数
    void (*mcu_reset)()//复位函数
);


EBUS_MCUBOOT_STATE_ENUM eBus_mcuboot_state_get(void);
uint32 eBus_flash_param_update_flag_get(void);





#endif
