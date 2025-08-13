/* absacc.h: header file that allows absolute variable location at C level */
/* Copyright 2006-2007 ARM Limited. All rights reserved.                       */
/* version 1.01 */

#ifndef __ABSACC_H__
#define __ABSACC_H__

#ifndef __at
#define __at(_addr) __attribute__ ((at(_addr)))

#endif

#ifndef __section
#define __section(_name) __attribute__ ((section(_name)))

#endif

//将绝对定位变量都放在这个文件内
#ifdef GLOBAL_MAIN
#define GLOBAL
#else
#define GLOBAL extern
#endif

#define PHYSICAL_RAM_ADDR  (0x20000000)
#define ABS_RAM_ADDR  (PHYSICAL_RAM_ADDR + 0x3000)
#define IIC_RAM_ADDR (0xF000)

typedef struct
{
    uint8_t IC_Type;
    uint8_t Main_Version;
    uint8_t Sub_Version;
    uint8_t ResType;
    uint8_t UserInfo[16];
    uint8_t RES[4];
} IC_INFO;


//数据大小
#define CONFIG_SIZE (128)
#define IC_INFO_SIZE (24)
#define FORE_INFO_SIZE (100)
#define COMMAND_SIZE (4)
#define MAX_SENSOR (16)
#define REF_SIZE (MAX_SENSOR*2)
#define DIFF_SIZE (MAX_SENSOR*2)
#define RAW_MAX_NUM (32)
#define RAW_SIZE ((uint16_t)MAX_SENSOR*RAW_MAX_NUM*2)
#define IIC_LOG_SIZE (56)
#define UN_USE_SIZE (520)
//地址分配
#define CONFIG_BASE (ABS_RAM_ADDR) //0x000
#define CONFIG_W_BASE (CONFIG_BASE+CONFIG_SIZE)//0x080
#define IC_INFO_BASE (CONFIG_W_BASE+CONFIG_SIZE)//0x100
#define FORCE_INFO_BASE (IC_INFO_BASE+IC_INFO_SIZE)//0x118
#define COMMAND_BASE (FORCE_INFO_BASE+FORE_INFO_SIZE)//0x17C
#define REF_BASE (COMMAND_BASE+COMMAND_SIZE)//0x180
#define DIFF_BASE (REF_BASE+REF_SIZE)//0x1A0
#define RAW_BASE (DIFF_BASE+DIFF_SIZE)//0x1C0
#define IIC_LOG_BASE (RAW_BASE+RAW_SIZE)//0x5C0
#define UN_USE_BASE (IIC_LOG_BASE+IIC_LOG_SIZE)//0x5F8


//定位变量

#ifndef SOURCE_INSIGHT//方便使用
GLOBAL uint8_t Config[CONFIG_SIZE] __at(CONFIG_BASE);//0x000
GLOBAL uint8_t ConfigW[CONFIG_SIZE] __at(CONFIG_W_BASE);//0x080
GLOBAL IC_INFO IC_Info __at(IC_INFO_BASE);//0x100
GLOBAL uint8_t Force_Info[FORE_INFO_SIZE] __at(FORCE_INFO_BASE);//0x118
GLOBAL uint8_t Command[COMMAND_SIZE] __at(COMMAND_BASE);//0x17C
GLOBAL uint16_t Ref[MAX_SENSOR] __at(REF_BASE);//0x180
GLOBAL int16_t Diff[MAX_SENSOR] __at(DIFF_BASE);//0x1A0
GLOBAL uint16_t Raw[RAW_MAX_NUM][MAX_SENSOR] __at(RAW_BASE);//0x1C0
GLOBAL uint8_t IIC_Log[IIC_LOG_SIZE] __at(IIC_LOG_BASE);//0x5F8
GLOBAL uint8_t UnUse[UN_USE_SIZE] __at(UN_USE_BASE);//0x640
#else
GLOBAL uint8_t Config[CONFIG_SIZE];//0x000
GLOBAL uint8_t ConfigW[CONFIG_SIZE];//0x080
GLOBAL IC_INFO IC_Info;//0x100
GLOBAL uint8_t Force_Info[FORE_INFO_SIZE];//0x118
GLOBAL uint8_t Command[COMMAND_SIZE];//0x17C
GLOBAL uint16_t Ref[MAX_SENSOR];//0x180
GLOBAL int16_t Diff[MAX_SENSOR];//0x1A0
GLOBAL uint16_t Raw[RAW_MAX_NUM][MAX_SENSOR];//0x1C0
GLOBAL uint8_t IIC_Log[IIC_LOG_SIZE];//0x5F8
GLOBAL uint8_t UnUse[UN_USE_SIZE];//0x640

#endif
//定义常量

#define USER_INFO_FLASH_BASE (0xE200)
#define IIC_ADDR_BASE (0xE220)
#define IIC_ADDR_LEN (4)


//定义版本号
#define IC_TYPE (0xCA) //bootloader
#define MAIN_VERSION (0x00)
#define SUB_VERSION (0x01)
#define FORCE_TYPE (0x01)// 1代表F61

#endif

