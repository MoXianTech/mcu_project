//UTF-8

#ifndef _EBUS_PORT_H_
#define _EBUS_PORT_H_

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


//数据长度
#define EBUS_PORT_MAX_PACK_LENGTH                        1100

//数据长度
#define EBUS_PORT_USER_RAM_LENGTH                        20



void eBus_init(void);
void eBus_run(uint16 time_ms);



uint8 eBus_flow_read_func(uint8 *value);
uint8 eBus_flow_write_func(uint8 value);
void sys_restart(void);








#define EBUS_MCU_APP_START_ADD                  0x8000000
//boot升级程序相关配置
//应用程序起始地址
#define EBUS_MCUBOOT_APP_START_ADD              0x8002000
//最大ROM可用空间 0x8000（32KB）
#define EBUS_MCUBOOT_APP_SIZE                   0x8000
//页长度
#define EBUS_MCUBOOT_PAGE_SIZE  1024


//掉电保存参数配置区
#define	EBUS_MCUBOOT_CONFIG_PARAM_ADD								0x801FC00
#define EBUS_MCUBOOT_CONFIG_PARAM_SIZE							1024
//启动标志
#define UPDATE_FLAG_ADD															EBUS_MCUBOOT_CONFIG_PARAM_ADD


#endif
