/**
* @file                framework_reg.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup Framework层
  * @{
  */
/**
 * @defgroup 数据交互模块
 * @brief 该模块通过IIC总线完成与主控的数据交互
 * @details
 * @{
 */
//hal
#include "hal_iic.h"
#include "hal_common.h"
#include "hal_afe.h"
#include "hal_tk.h"

//framework
#include "framework_reg.h"
#include "framework_sample.h"
#include "framework_system.h"
#include "framework_pmu.h"
#include "framework_notify.h"
#include "framework_debug.h"

//config
#include "user_config.h"

#define BIT(X) ((uint32_t)(1)<<(X))
#define ABS(x)  (((x)>0) ? (x) : (0-(x)))


//寄存器表
const TS_I2C_REG REG_TABLE[] =
{

    //寄存器，            操作方式，          参数个数   BUFF地址
    {REG_NOP,             REG_OP_COMMAND,         0, 				NULL},     //reg 0x00
    {REG_RESET_MCU,       REG_OP_COMMAND,         0, 				NULL},      //reg 0x01
    {REG_DEVICE_ID,       REG_OP_READ_ONLY,      10,				Reg_DeviceID},     //reg 0x02
    {REG_MANUFACTURER_ID, REG_OP_READ_ONLY,       2,				Reg_ManufacturerID},     //reg 0x03
    {REG_MODULE_ID,       REG_OP_READ_ONLY,       2,				Reg_ModuleID},     //reg 0x04
    {REG_FW_VERSION,      REG_OP_READ_ONLY,       2,				Reg_FWVersion},     //reg 0x05
    {REG_WAKE_UP,         REG_OP_COMMAND,         0, 				NULL},     //reg 0x06
    {REG_SLEEP,           REG_OP_COMMAND,         0, 				NULL},     //reg 0x07
    {0x08,                0x00,                   0, 				NULL},
    {REG_RESET_PGA,       REG_OP_COMMAND,         0, 				NULL},     //reg 0x09
    {REG_TASK_ENABLE,     REG_OP_READ_WRITE,      2, 				Reg_TaskEnable},
    {0x0B,                0x00,                   0, 				NULL},
    {0x0C,                0x00,                   0, 				NULL},
    {0x0D,                0x00,                   0, 				NULL},
    {REG_SW_ENABLE,       REG_OP_READ_WRITE,      1, 				Reg_SW_enable},
    {REG_SW_DISABLE,      REG_OP_WRITE_ONLY,      1, 				Reg_SW_disable},
    //Force touch register
    {REG_POINTER_NUMBER,  REG_OP_WRITE_ONLY,      1, 				Reg_PointerNumber},     //reg 0x10
    {REG_POINTER_DATA1,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData1},     //reg 0x11
    {REG_POINTER_DATA2,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData2},     //reg 0x12
    {REG_POINTER_DATA3,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData3},     //reg 0x13
    {REG_POINTER_DATA4,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData4},     //reg 0x14
    {REG_POINTER_DATA5,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData5},     //reg 0x15
    {REG_POINTER_DATA6,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData6},     //reg 0x16
    {REG_POINTER_DATA7,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData7},     //reg 0x17
    {REG_POINTER_DATA8,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData8},     //reg 0x18
    {REG_POINTER_DATA9,   REG_OP_WRITE_ONLY,      5, 				Reg_PointerData9},     //reg 0x19
    {REG_POINTER_DATA10,  REG_OP_WRITE_ONLY,      5, 				Reg_PointerData10},     //reg 0x1A

    {0x1B,                0x00,                   0, 				NULL},     //reg 0x1B
    {0x1C,                0x00,                   0, 				NULL},     //reg 0x1C
    {0x1D,                0x00,                   0, 				NULL},     //reg 0x1D
    {0x1E,                0x00,                   0, 				NULL},     //reg 0x1E
    //  {0x1F,                0x00,                   0},    //reg 0x1F
    // {0x1F,                REG_OP_READ_WRITE,     10},     //reg 1F, for write/read test
    {REG_RD_TEST,			REG_OP_READ_WRITE,      1, 				Reg_RDtest},      //modified by Eric.Chen
    {REG_FORCEDATA_NUMBER, REG_OP_READ_WRITE,      1, 			    Reg_ForceDataNumber},    //reg 0x20
    {REG_FORCE_DATA1,     REG_OP_READ_ONLY,       2, 				Reg_ForceData1},     //reg 0x21
    {REG_FORCE_DATA2,     REG_OP_READ_ONLY,       2, 				Reg_ForceData2},     //reg 0x22
    {REG_FORCE_DATA3,     REG_OP_READ_ONLY,       2, 				Reg_ForceData3},     //reg 0x23
    {REG_FORCE_DATA4,     REG_OP_READ_ONLY,       2, 				Reg_ForceData4},     //reg 0x24
    {REG_FORCE_DATA5,     REG_OP_READ_ONLY,       2, 				Reg_ForceData5},     //reg 0x25
    {REG_FORCE_DATA6,     REG_OP_READ_ONLY,       2, 				Reg_ForceData6},     //reg 0x26
    {REG_FORCE_DATA7,     REG_OP_READ_ONLY,       2, 				Reg_ForceData7},     //reg 0x27
    {REG_FORCE_DATA8,     REG_OP_READ_ONLY,       2, 				Reg_ForceData8},     //reg 0x28
    {REG_FORCE_DATA9,     REG_OP_READ_ONLY,       2, 				Reg_ForceData9},     //reg 0x29
    {REG_FORCE_DATA10,    REG_OP_READ_ONLY,       2, 				Reg_ForceData10},     //reg 0x2A

    {REG_SW_IOSTATE,      REG_OP_READ_WRITE,      1, 				Reg_SWState},     //reg 0x2B用于查询SW调试口状态，0 - SW关闭，1 - 打开
    {REG_GPIO_DISABLE,    REG_OP_READ_WRITE,      1, 				Reg_GPIO_Disable},
    {REG_UART_ENABLE,     REG_OP_READ_WRITE,      1, 				Reg_UART_Enable},
    {REG_IIC_PRINTF_ENABLE,     REG_OP_READ_WRITE,      1, 				Reg_IIC_Printf_Enable},
    {0x2F,                0x00,                   0, 				NULL},
    {0x30,                0x00,                   0, 				NULL},
    {0x31,                0x00,                   0, 				NULL},
    {0x32,                0x00,                   0, 				NULL},
    {0x33,                0x00,                   0, 				NULL},
    {0x34,                0x00,                   0, 				NULL},
    {0x35,                0x00,                   0, 				NULL},
    {0x36,                0x00,                   0, 				NULL},
    {0x37,                0x00,                   0, 				NULL},
    {0x38,                0x00,                   0, 				NULL},
    {0x39,                0x00,                   0, 				NULL},
    {0x3A,                0x00,                   0, 				NULL},
    {0x3B,                0x00,                   0, 				NULL},
    {0x3C,                0x00,                   0, 				NULL},
    {0x3D,                0x00,                   0, 				NULL},
    {0x3E,                0x00,                   0, 				NULL},
    {0x3F,                0x00,                   0, 				NULL},
    {0x40,                0x00,                   0, 				NULL},
    {0x41,                0x00,                   0, 				NULL},
    {0x42,                0x00,                   0, 				NULL},
    {0x43,                0x00,                   0, 				NULL},
    {0x44,                0x00,                   0, 				NULL},
    {0x45,                0x00,                   0, 				NULL},
    {0x46,                0x00,                   0, 				NULL},
    {0x47,                0x00,                   0, 				NULL},
    {0x48,                0x00,                   0, 				NULL},
    {0x49,                0x00,                   0, 				NULL},
    {0x4A,                0x00,                   0, 				NULL},
    {0x4B,                0x00,                   0, 				NULL},
    {0x4C,                0x00,                   0, 				NULL},
    {0x4D,                0x00,                   0, 				NULL},
    {REG_SENSOR_STATE,	  REG_OP_READ_ONLY,		  2, 				Reg_SensorState},
    {REG_IO_STATE,        REG_OP_COMMAND,         2, 				Reg_IoState},

    {REG_HOST_STATUS,     REG_OP_READ_WRITE,      1, 				Reg_HostStatus},     //reg 0x50
    {REG_BATTERY_VOLTAGE, REG_OP_WRITE_ONLY,      2, 				Reg_BatteryVoltage},     //reg 0x51
    {REG_TEMPERATURE,     REG_OP_READ_ONLY,       18, 			Reg_Temperature},     //reg 0x52
    {0x53,                0x00,                   0, 				NULL},
    {0x54,                0x00,                   0, 				NULL},
    {0x55,                0x00,                   0, 				NULL},
    {0x56,                0x00,                   0, 				NULL},
    {0x57,                0x00,                   0, 				NULL},
    {0x58,                0x00,                   0, 				NULL},
    {0x59,                0x00,                   0, 				NULL},
    {0x5A,                0x00,                   0, 				NULL},
    {0x5B,                0x00,                   0, 				NULL},
    {0x5C,                0x00,                   0, 				NULL},
    {0x5D,                0x00,                   0, 				NULL},
    {0x5E,                0x00,                   0, 				NULL},
    {0x5F,                0x00,                   0, 				NULL},
    //Debug register
    {REG_DEBUG_MODE,      REG_OP_READ_WRITE,      1, 				Reg_DebugMode},     //reg 0x60
    {REG_DATA_READY,      REG_OP_READ_WRITE,      1, 				Reg_DataReady},     //reg 0x61
    {REG_DEBUG_DATA1,     REG_OP_READ_WRITE,      255, 			Reg_DebugData1},     //reg 0x62
    {REG_DEBUG_DATA2,     REG_OP_READ_WRITE,      2, 				Reg_DebugData2},     //reg 0x63
    {REG_DEBUG_DATA3,     REG_OP_READ_WRITE,      2, 				Reg_DebugData3},     //reg 0x64
    {REG_DEBUG_DATA4,     REG_OP_READ_WRITE,      2, 				Reg_DebugData4},     //reg 0x65
    {REG_DEBUG_DATA5,     REG_OP_READ_WRITE,      2, 				Reg_DebugData5},     //reg 0x66
    {REG_DEBUG_DATA6,     REG_OP_READ_WRITE,      2, 				Reg_DebugData6},     //reg 0x67
    {REG_DEBUG_DATA7,     REG_OP_READ_WRITE,      2, 				Reg_DebugData7},     //reg 0x68
    {REG_DEBUG_DATA8,     REG_OP_READ_WRITE,      2, 				Reg_DebugData8},     //reg 0x69
    {REG_DEBUG_DATA9,     REG_OP_READ_WRITE,      2, 				Reg_DebugData9},     //reg 0x6A
    {REG_DEBUG_DATA10,    REG_OP_READ_WRITE,      2, 				Reg_DebugData10},     //reg 0x6B
    {REG_DEBUG_DATA11,    REG_OP_READ_WRITE,      2, 				Reg_DebugData11},     //reg 0x6C
    {REG_DEBUG_DATA12,    REG_OP_READ_WRITE,      2, 				Reg_DebugData12},     //reg 0x6D
    {REG_DEBUG_DATA13,    REG_OP_READ_WRITE,      2, 				Reg_DebugData13},     //reg 0x6E
    {REG_DEBUG_DATA14,    REG_OP_READ_WRITE,      2, 				Reg_DebugData14},     //reg 0x6F
    {REG_DEBUG_DATA15,    REG_OP_READ_WRITE,      2, 				Reg_DebugData15},     //reg 0x70
    {REG_DEBUG_DATA16,    REG_OP_READ_WRITE,      2, 				Reg_DebugData16},     //reg 0x71
    {REG_DEBUG_DATA17,    REG_OP_READ_WRITE,      2, 				Reg_DebugData17},     //reg 0x72
    {REG_DEBUG_DATA18,    REG_OP_READ_WRITE,      2, 				Reg_DebugData18},     //reg 0x73
    {REG_DEBUG_DATA19,    REG_OP_READ_WRITE,      2, 				Reg_DebugData19},     //reg 0x74
    {REG_DEBUG_DATA20,    REG_OP_READ_WRITE,      2, 				Reg_DebugData20},     //reg 0x75
    {REG_DEBUG_DATA21,    REG_OP_READ_WRITE,      2, 				Reg_DebugData21},     //reg 0x76
    {REG_DEBUG_DATA22,    REG_OP_READ_WRITE,      2, 				Reg_DebugData22},     //reg 0x77
    {REG_DEBUG_DATA23,    REG_OP_READ_WRITE,      2, 				Reg_DebugData23},     //reg 0x78
    {REG_DEBUG_DATA24,    REG_OP_READ_WRITE,      2, 				Reg_DebugData24},     //reg 0x79
    {REG_DEBUG_DATA25,    REG_OP_READ_WRITE,      2, 				Reg_DebugData25},     //reg 0x7A
    {REG_DEBUG_DATA26,    REG_OP_READ_WRITE,      2, 				Reg_DebugData26},     //reg 0x7B
    {REG_DEBUG_DATA27,    REG_OP_READ_WRITE,      2, 				Reg_DebugData27},     //reg 0x7C
    {REG_DEBUG_DATA28,    REG_OP_READ_WRITE,      2, 				Reg_DebugData28},     //reg 0x7D
    {REG_DEBUG_DATA29,    REG_OP_READ_WRITE,      2, 				Reg_DebugData29},     //reg 0x7E
    {REG_DEBUG_DATA30,    REG_OP_READ_WRITE,      2, 				Reg_DebugData30},     //reg 0x7F

    {REG_DEBUG_MODE_B,    REG_OP_READ_WRITE,      1, 				Reg_DebugModeB},
    {REG_DATA_READY_B,    REG_OP_READ_WRITE,      1, 				Reg_DataReadyB},
    {REG_DEBUG_DATA_B,    REG_OP_READ_WRITE,      255, 			Reg_DebugDataB},
    {REG_DEBUG_MODE_C,    REG_OP_READ_WRITE,      1, 				Reg_DebugModeC},
    {REG_DATA_READY_C,    REG_OP_READ_WRITE,      1, 				Reg_DataReadyC},
    {REG_DEBUG_DATA_C,    REG_OP_READ_WRITE,      255, 			Reg_DebugDataC},
    {REG_DEBUG_MODE_D,    REG_OP_READ_WRITE,      1, 				Reg_DebugModeD},
    {REG_DATA_READY_D,    REG_OP_READ_WRITE,      1, 				Reg_DataReadyD},
    {REG_DEBUG_DATA_D,    REG_OP_READ_WRITE,      255, 			Reg_DebugDataD},
    {REG_DEBUG_MODE_E,    REG_OP_READ_WRITE,      1, 				Reg_DebugModeE},
    {REG_DATA_READY_E,    REG_OP_READ_WRITE,      1, 				Reg_DataReadyE},
    {REG_DEBUG_DATA_E,    REG_OP_READ_WRITE,      255, 			Reg_DebugDataE},
    {0x8C,                0x00,                   0, 				NULL},
    {0x8D,                0x00,                   0, 				NULL},
    {0x8E,                0x00,                   0, 				NULL},
    {0x8F,                0x00,                   0, 				NULL},
    {0x90,                0x00,                   0, 				NULL},
    {0x91,                0x00,                   0, 				NULL},
    {0x92,                0x00,                   0, 				NULL},
    {0x93,                0x00,                   0, 				NULL},
    {0x94,                0x00,                   0, 				NULL},
    {0x95,                0x00,                   0, 				NULL},
    {0x96,                0x00,                   0, 				NULL},
    {0x97,                0x00,                   0, 				NULL},
    {0x98,                0x00,                   0, 				NULL},
    {0x99,                0x00,                   0, 				NULL},
    {0x9A,                0x00,                   0, 				NULL},
    {0x9B,                0x00,                   0, 				NULL},
    {0x9C,                0x00,                   0, 				NULL},
    {0x9D,                0x00,                   0, 				NULL},
    {0x9E,                0x00,                   0, 				NULL},
    {0x9F,                0x00,                   0, 				NULL},
    {0xA0,                0x00,                   0, 				NULL},
    {0xA1,                0x00,                   0, 				NULL},
    {0xA2,                0x00,                   0, 				NULL},
    {0xA3,                0x00,                   0, 				NULL},
    {0xA4,                0x00,                   0, 				NULL},
    {0xA5,                0x00,                   0, 				NULL},
    {0xA6,                0x00,                   0, 				NULL},
    {0xA7,                0x00,                   0, 				NULL},
    {0xA8,                0x00,                   0, 				NULL},
    {0xA9,                0x00,                   0, 				NULL},
    {0xAA,                0x00,                   0, 				NULL},
    {0xAB,                0x00,                   0, 				NULL},
    {0xAC,                0x00,                   0, 				NULL},
    {0xAD,                0x00,                   0, 				NULL},
    {0xAE,                0x00,                   0, 				NULL},
    {0xAF,                0x00,                   0, 				NULL},
    {0xB0,                0x00,                   0, 				NULL},
    {0xB1,                0x00,                   0, 				NULL},
    {0xB2,                0x00,                   0, 				NULL},
    {0xB3,                0x00,                   0, 				NULL},
    {0xB4,                0x00,                   0, 				NULL},
    {0xB5,                0x00,                   0, 				NULL},
    {0xB6,                0x00,                   0, 				NULL},
    {0xB7,                0x00,                   0, 				NULL},
    {0xB8,                0x00,                   0, 				NULL},
    {0xB9,                0x00,                   0, 				NULL},
    {0xBA,                0x00,                   0, 				NULL},
    {0xBB,                0x00,                   0, 				NULL},
    {0xBC,                0x00,                   0, 				NULL},
    {0xBD,                0x00,                   0, 				NULL},
    {0xBE,                0x00,                   0, 				NULL},
    {0xBF,                0x00,                   0, 				NULL},
    {0xC0,                0x00,                   0, 				NULL},
    {0xC1,                0x00,                   0, 				NULL},
    {0xC2,                0x00,                   0, 				NULL},
    {0xC3,                0x00,                   0, 				NULL},
    {0xC4,                0x00,                   0, 				NULL},
    {0xC5,                0x00,                   0, 				NULL},
    {0xC6,                0x00,                   0, 				NULL},
    {0xC7,                0x00,                   0, 				NULL},
    {0xC8,                0x00,                   0, 				NULL},
    {0xC9,                0x00,                   0, 				NULL},
    {0xCA,                0x00,                   0, 				NULL},
    {0xCB,                0x00,                   0, 				NULL},
    {0xCC,                0x00,                   0, 				NULL},
    {0xCD,                0x00,                   0, 				NULL},
    {0xCE,                0x00,                   0, 				NULL},
    {0xCF,                0x00,                   0, 				NULL},
    {0xD0,                0x00,                   0, 				NULL},
    {0xD1,                0x00,                   0, 				NULL},
    {0xD2,                0x00,                   0, 				NULL},
    {REG_KEY_OUTPUT,      REG_OP_READ_WRITE,      5, 				Reg_KeyOutput},
    {0xD4,                0x00,                   0, 				NULL},
    {0xD5,                0x00,                   0, 				NULL},
    {0xD6,                0x00,                   0, 				NULL},
    {0xD7,                0x00,                   0, 				NULL},
    {0xD8,                0x00,                   0, 				NULL},
    {0xD9,                0x00,                   0, 				NULL},
    {0xDA,                0x00,                   0, 				NULL},
    {0xDB,                0x00,                   0, 				NULL},
    {0xDC,                0x00,                   0, 				NULL},
    {0xDD,                0x00,                   0, 				NULL},
    {0xDE,                0x00,                   0, 				NULL},
    {0xDF,                0x00,                   0, 				NULL},
    {0xE0,                0x00,                   0, 				NULL},
    {0xE1,                0x00,                   0, 				NULL},
    {0xE2,                0x00,                   0, 				NULL},
    {0xE3,                0x00,                   0, 				NULL},
    {0xE4,                0x00,                   0, 				NULL},
    {0xE5,                0x00,                   0, 				NULL},
    {0xE6,                0x00,                   0, 				NULL},
    {0xE7,                0x00,                   0, 				NULL},
    {0xE8,                0x00,                   0, 				NULL},
    {0xE9,                0x00,                   0, 				NULL},
    {0xEA,                0x00,                   0, 				NULL},
    {0xEB,                0x00,                   0, 				NULL},
    {0xEC,                0x00,                   0, 				NULL},
    {0xED,                0x00,                   0, 				NULL},
    {0xEE,                0x00,                   0, 				NULL},
    {0xEF,                0x00,                   0, 				NULL},
    {0xF0,                0x00,                   0, 				NULL},
    {0xF1,                0x00,                   0, 				NULL},
    {0xF2,                0x00,                   0, 				NULL},
    {0xF3,                0x00,                   0, 				NULL},
    {0xF4,                0x00,                   0, 				NULL},
    {0xF5,                0x00,                   0, 				NULL},
    {0xF6,                0x00,                   0, 				NULL},
    {0xF7,                0x00,                   0, 				NULL},
    {0xF8,                0x00,                   0, 				NULL},
    {0xF9,                0x00,                   0, 				NULL},
    {0xFA,                0x00,                   0, 				NULL},
    {0xFB,                0x00,                   0, 				NULL},
    {0xFC,                0x00,                   0, 				NULL},
    {0xFD,                0x00,                   0, 				NULL},
    {0xFE,                0x00,                   0, 				NULL},
    {0xFF,                0x00,                   0, 				NULL},
};



unsigned char Reg_DeviceID[10]   = {0x00};
unsigned char Reg_ManufacturerID[] = {MANUFACTURER_ID_LSB, MANUFACTURER_ID_MSB};
unsigned char Reg_ModuleID[]   = {MODULE_ID_LSB, MODULE_ID_MSB};
unsigned char Reg_FWVersion[]  = {FW_VERSION_LSB, FW_VERSION_MSB};
unsigned char Reg_TaskEnable[] = {0xFF, 0xFF};
__align(2)unsigned char Reg_SensorState[2] = {0x00};
__align(2)unsigned char Reg_IoState[2] = {0x00};

unsigned char Reg_DebugMode[1]  = {0x00};
unsigned char Reg_DataReady[1]  = {0x00};
__align(4) unsigned char Reg_DebugData1[256] = {0x00};
__align(4) unsigned char Reg_DebugData2[2] = {0x00};
__align(4) unsigned char Reg_DebugData3[2] = {0x00};
__align(4) unsigned char Reg_DebugData4[2] = {0x00};
__align(4) unsigned char Reg_DebugData5[2] = {0x00};
__align(4) unsigned char Reg_DebugData6[2] = {0x00};
__align(4) unsigned char Reg_DebugData7[2] = {0x00};
__align(4) unsigned char Reg_DebugData8[2] = {0x00};
__align(4) unsigned char Reg_DebugData9[2] = {0x00};
__align(4) unsigned char Reg_DebugData10[2] = {0x00};
__align(4) unsigned char Reg_DebugData11[2] = {0x00};
__align(4) unsigned char Reg_DebugData12[2] = {0x00};
__align(4) unsigned char Reg_DebugData13[2] = {0x00};
__align(4) unsigned char Reg_DebugData14[2] = {0x00};
__align(4) unsigned char Reg_DebugData15[2] = {0x00};
unsigned char Reg_DebugData16[2] = {0x00};
unsigned char Reg_DebugData17[2] = {0x00};
unsigned char Reg_DebugData18[2] = {0x00};
unsigned char Reg_DebugData19[2] = {0x00};
unsigned char Reg_DebugData20[2] = {0x00};
unsigned char Reg_DebugData21[2] = {0x00};
unsigned char Reg_DebugData22[2] = {0x00};
unsigned char Reg_DebugData23[2] = {0x00};
unsigned char Reg_DebugData24[2] = {0x00};
unsigned char Reg_DebugData25[2] = {0x00};
unsigned char Reg_DebugData26[2] = {0x00};
unsigned char Reg_DebugData27[2] = {0x00};
unsigned char Reg_DebugData28[2] = {0x00};
unsigned char Reg_DebugData29[2] = {0x00};
unsigned char Reg_DebugData30[2] = {0x00};
unsigned char Reg_DebugModeB[1]  = {0x00};
unsigned char Reg_DataReadyB[1]  = {0x00};
__align(4) unsigned char Reg_DebugDataB[256] = {0x00};
unsigned char Reg_DebugModeC[1]  = {0x00};
unsigned char Reg_DataReadyC[1]  = {0x00};
__align(4) unsigned char Reg_DebugDataC[256] = {0x00};
unsigned char Reg_DebugModeD[1]  = {0x00};
unsigned char Reg_DataReadyD[1]  = {0x00};
__align(4) unsigned char Reg_DebugDataD[256] = {0x00};
unsigned char Reg_DebugModeE[1]  = {0x00};
unsigned char Reg_DataReadyE[1]  = {0x00};
__align(4) unsigned char Reg_DebugDataE[256] = {0x00};

unsigned char Reg_PointerNumber[1] = {0x00};
unsigned char Reg_PointerData1[5] = {0x00};
unsigned char Reg_PointerData2[5] = {0x00};
unsigned char Reg_PointerData3[5] = {0x00};
unsigned char Reg_PointerData4[5] = {0x00};
unsigned char Reg_PointerData5[5] = {0x00};
unsigned char Reg_PointerData6[5] = {0x00};
unsigned char Reg_PointerData7[5] = {0x00};
unsigned char Reg_PointerData8[5] = {0x00};
unsigned char Reg_PointerData9[5] = {0x00};
unsigned char Reg_PointerData10[5] = {0x00};

//press data reg
unsigned char Reg_ForceDataNumber[1] = {0x00};
__align(2) unsigned char Reg_ForceData1[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData2[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData3[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData4[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData5[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData6[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData7[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData8[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData9[2] = {0x01, 0x00};
__align(2) unsigned char Reg_ForceData10[2] = {0x01, 0x00};


unsigned char Reg_HostStatus[1] = {0x00};

unsigned char Reg_BatteryVoltage[2] = {0x00};
__align(4) unsigned char Reg_Temperature[18] = {0xFF, 0xFF, 0x00, 0x04, 0x00, 0x04};

unsigned char Reg_KeySensitivity[1] = {0x00};
unsigned char Reg_KeySimulate[2] = {0xFF, 0xFF};
unsigned char Reg_KeyOutput[5] = {0x00};
unsigned char Reg_UARTPrintEnable[2] = {0x00};

unsigned char Reg_DefaultData[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

unsigned char Reg_WRBuff[10] = {0x00};

uint8_t Reg_RDtest[1]  = {0x00};
uint8_t Reg_SWState[1] = {0x01};         //出厂设置开启SW？？？？？？？？？？？？？？？？？？？？

uint8_t Reg_GPIO_Disable[1] = {0x00};
uint8_t Reg_UART_Enable[1] = {DEBUG_PRINT_DEFAULT_OUTPUT};
uint8_t Reg_IIC_Printf_Enable[1] = {DEBUG_PRINT_IIC_DEFAULT_OUTPUT};

unsigned char Register;

unsigned char *Reg_DebugModeNew;
unsigned char *Reg_DataReadyNew;
unsigned char *Reg_DebugDataNew;

unsigned char Reg_SW_enable[1] = {0x00};
unsigned char Reg_SW_disable[1] = {0x00};

//iic
uint8_t I2C_AddrFlag = 0;

uint8_t I2C_wPointer = 0;

uint8_t I2C_rPointer = 0;

//app reg data
static int16_t fw_reg_app_rawdata[FW_SAMPLE_RAWDATA_MAX];
static int16_t fw_reg_app_forcedata[FW_SAMPLE_RAWDATA_MAX];
static int16_t fw_reg_app_baseline[FW_SAMPLE_RAWDATA_MAX];

//--function-----------------------------------
static void fw_reg_iic_state_init(void);

static void (*fun_sw_enable[8])(void);
static void (*fun_sw_disable[8])(void);

static uint32_t fw_reg_disable_time_count = 0;          // 禁止IO输出的计时变量(兼容旧协议)

// rawdtata full
#define FW_REG_RAWDATA_FULL_MAX_FRAME       5
static int16_t fw_reg_rawdata_full_buffer[FW_SAMPLE_RAWDATA_MAX*2*FW_REG_RAWDATA_FULL_MAX_FRAME];
static uint8_t fw_reg_rawdata_full_set_count = 0;

/**
* @fn void fw_reg_null(void)
* @brief fw_reg_null
* @param None
* @return None.
*/
static void fw_reg_null(void)
{

}

/**
* @fn void fw_reg_init(void)
* @brief reg system init
* @param None
* @return None.
*/
void fw_reg_init(void)
{
    hal_iic_init();                                 // iic硬件初始化

    hal_common_get_chip_ID(Reg_DeviceID, 8);      //  获取芯片硬件ID
    Reg_DeviceID[8] = 0XBE;
    Reg_DeviceID[9] = 0XEF;

    fw_reg_iic_state_init();                        // 通讯变量初始化

    fun_sw_enable[0] = fw_notify_int_out_enable;
    fun_sw_enable[1] = fw_notify_key_out_enable;
    fun_sw_enable[2] = fw_notify_motor_trig_enable;
    fun_sw_enable[3] = fw_reg_null;
    fun_sw_enable[4] = fw_reg_null;
    fun_sw_enable[5] = fw_printf_iic_out_enable;
    fun_sw_enable[6] = fw_printf_out_enable;
    fun_sw_enable[7] = fw_reg_null;

    fun_sw_disable[0] = fw_notify_int_out_disable;
    fun_sw_disable[1] = fw_notify_key_out_disable;
    fun_sw_disable[2] = fw_notify_motor_trig_disable;
    fun_sw_disable[3] = fw_reg_null;
    fun_sw_disable[4] = fw_reg_null;
    fun_sw_disable[5] = fw_printf_iic_out_disable;
    fun_sw_disable[6] = fw_printf_out_disable;
    fun_sw_disable[7] = fw_reg_null;

}

/**
* @fn static void fw_reg_iic_state_init(void)
* @brief iic state init
* @param None
* @return None.
*/
static void fw_reg_iic_state_init(void)
{
    I2C_AddrFlag = YES;
}

/**
* @fn void fw_reg_set(uint8_t addr, uint8_t offset, uint8_t dat)
* @brief set reg data
* @param [in]addr: reg addr, [in]offset: reg offset, [in]dat: write data
* @return None.
*/
void fw_reg_set(uint8_t addr, uint8_t offset, uint8_t dat)
{
    REG_TABLE[addr].p_Cmd_Buf[offset] = dat;
}

/**
* @fn uint8_t fw_reg_get(uint8_t addr, uint8_t offset)
* @brief get reg data
* @param [in]addr: reg addr, [in]offset: reg offset
* @return reg data
*/
uint8_t fw_reg_get(uint8_t addr, uint8_t offset)
{
    return REG_TABLE[addr].p_Cmd_Buf[offset];
}

/**
* @fn void fw_reg_set(uint8_t addr, uint8_t offset, uint8_t dat)
* @brief set reg data
* @param [in]addr: reg addr, [in]offset: reg offset, [in]dat: write data
* @return None.
*/
void fw_reg_set_word(uint8_t addr, uint8_t offset, uint16_t dat)
{
    uint16_t *lpData;

    lpData = (uint16_t *)&REG_TABLE[addr].p_Cmd_Buf[offset];
    *lpData = dat;
}

/**
* @fn uint16_t fw_reg_get(uint8_t addr, uint8_t offset)
* @brief get reg data
* @param [in]addr: reg addr, [in]offset: reg offset
* @return reg data
*/
uint16_t fw_reg_get_word(uint8_t addr, uint8_t offset)
{
    uint16_t *lpData;

    lpData = (uint16_t *)&REG_TABLE[addr].p_Cmd_Buf[offset];
    return *lpData;
}

/**
* @fn void fw_reg_set_dword(uint8_t addr, uint8_t offset, uint8_t dat)
* @brief set reg data
* @param [in]addr: reg addr, [in]offset: reg offset, [in]dat: write data
* @return None.
*/
void fw_reg_set_dword(uint8_t addr, uint8_t offset, uint32_t dat)
{
    uint32_t *lpData;

    lpData = (uint32_t *)&REG_TABLE[addr].p_Cmd_Buf[offset];
    *lpData = dat;
}

/**
* @fn uint32_t fw_reg_get_dword(uint8_t addr, uint8_t offset)
* @brief get reg data
* @param [in]addr: reg addr, [in]offset: reg offset
* @return reg data
*/
uint32_t fw_reg_get_dword(uint8_t addr, uint8_t offset)
{
    uint32_t *lpData;

    lpData = (uint32_t *)&REG_TABLE[addr].p_Cmd_Buf[offset];
    return *lpData;
}


/**
* @fn fw_reg_set_press_reg(uint8_t key_num, uint16_t press_data)
* @brief set press reg
* @param [in]key_num: key num, [in]press_data: press data
* @return None.
*/
void fw_reg_set_press_reg(uint8_t key_num, uint16_t press_data)             //设置按键n的压力寄存器值
{
    if(key_num < 8)
    {
        Reg_ForceDataNumber[0] |= BIT(key_num);

        switch(key_num)
        {
        case 0:
            *((uint16_t *)Reg_ForceData1) = press_data;
            break;

        case 1:
            *((uint16_t *)Reg_ForceData2) = press_data;
            break;

        case 2:
            *((uint16_t *)Reg_ForceData3) = press_data;
            break;

        case 3:
            *((uint16_t *)Reg_ForceData4) = press_data;
            break;

        case 4:
            *((uint16_t *)Reg_ForceData5) = press_data;
            break;

        case 5:
            *((uint16_t *)Reg_ForceData6) = press_data;
            break;

        case 6:
            *((uint16_t *)Reg_ForceData7) = press_data;
            break;

        case 7:
            *((uint16_t *)Reg_ForceData8) = press_data;
            break;

        default:
            break;
        }
    }

}

/**
* @fn void fw_reg_clear_press_reg(uint8_t key_num)
* @brief clear press reg
* @param [in]key_um: key num
* @return None.
*/
void fw_reg_clear_press_reg(uint8_t key_num)                //清除按键n的压力寄存器值
{
    if(key_num < 8)
    {
        Reg_ForceDataNumber[0] &= ~BIT(key_num);

        switch(key_num)
        {
        case 0:
            *((uint16_t *)Reg_ForceData1) = 0;
            break;

        case 1:
            *((uint16_t *)Reg_ForceData2) = 0;
            break;

        case 2:
            *((uint16_t *)Reg_ForceData3) = 0;
            break;

        case 3:
            *((uint16_t *)Reg_ForceData4) = 0;
            break;

        case 4:
            *((uint16_t *)Reg_ForceData5) = 0;
            break;

        case 5:
            *((uint16_t *)Reg_ForceData6) = 0;
            break;

        case 6:
            *((uint16_t *)Reg_ForceData7) = 0;
            break;

        case 7:
            *((uint16_t *)Reg_ForceData8) = 0;
            break;

        default:
            break;
        }
    }

}

/**
* @fn void fw_reg_clear_all_press_reg(void)
* @brief clear all press reg
* @param None
* @return None.
*/
void fw_reg_clear_all_press_reg(void)               //清除所有按键压力寄存器值
{
    Reg_ForceDataNumber[0] = 0;

    *((uint16_t *)Reg_ForceData1) = 0;
    *((uint16_t *)Reg_ForceData2) = 0;
    *((uint16_t *)Reg_ForceData3) = 0;
    *((uint16_t *)Reg_ForceData4) = 0;
    *((uint16_t *)Reg_ForceData5) = 0;
    *((uint16_t *)Reg_ForceData6) = 0;
    *((uint16_t *)Reg_ForceData7) = 0;
    *((uint16_t *)Reg_ForceData8) = 0;
}

/**
* @fn uint8_t fw_reg_get_point_num(void)
* @brief get host set touch point num
* @param None
* @return touch point
*/
uint8_t fw_reg_get_point_num(void)          //返回主控下发的触摸点个数
{
    return Reg_PointerNumber[0];
}

/**
* @fn void fw_reg_get_point_coord(uint8_t point_num, uint16_t *x_coord, uint16_t *y_coord, uint8_t *press_status)
* @brief get host set touch point coord
* @param [in]point_num: point num, [out]x_coord: point to x coord, [out]y_coord: point to y coord, [out]press_status:point to press status
* @return None.
*/
void fw_reg_get_point_coord(uint8_t point_num, uint16_t *x_coord, uint16_t *y_coord, uint8_t *press_status) //获取主控下发的触摸点坐标及触摸状态
{
    if(point_num < 8)
    {
        switch(point_num)
        {
        case 0:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;
        case 1:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;
        case 2:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;
        case 3:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;
        case 4:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;
        case 5:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;
        case 6:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;
        case 7:
            *x_coord = *((uint16_t *)Reg_PointerData1);
            *y_coord = *((uint16_t *)&Reg_PointerData1[2]);
            *press_status = Reg_PointerData1[4];
            break;

        default:
            break;
        }
    }
}

/**
* @fn uint8_t fw_reg_set_app_rawdata(uint8_t num, int16_t dat)
* @brief app set output rawdata
* @param [in]data_buf: output data, [in]num: num
* @return success stauts， 0: success, 1:fail
*/
uint8_t fw_reg_set_app_rawdata(uint8_t num, int16_t dat)   // 设置app要输出的rawdata数据
{
    if(num < FW_SAMPLE_RAWDATA_MAX)
    {
        fw_reg_app_rawdata[num] = dat;

        return 0;
    }

    return 1;
}

/**
* @fn uint8_t fw_reg_set_app_forcedata(uint8_t num, int16_t dat)
* @brief app set output forcedata
* @param [in]data_buf: output data, [in]num: num
* @return success stauts， 0: success, 1:fail
*/
uint8_t fw_reg_set_app_forcedata(uint8_t num, int16_t dat) // 设置app要输出的forcedata数据
{
    if(num < FW_SAMPLE_RAWDATA_MAX)
    {
        fw_reg_app_forcedata[num] = dat;

        return 0;
    }

    return 1;
}

/**
* @fn uint8_t fw_reg_set_app_baseline(uint8_t num, int16_t dat)
* @brief app set output baseline
* @param [in]data_buf: output data, [in]num: num
* @return success stauts， 0: success, 1:fail
*/
uint8_t fw_reg_set_app_baseline(uint8_t num, int16_t dat)   // 设置app要输出的baseline数据
{
    if(num < FW_SAMPLE_RAWDATA_MAX)
    {
        fw_reg_app_baseline[num] = dat;

        return 0;
    }

    return 1;
}

/**
* @fn void fw_reg_set_sensor_status(uint16_t ch, uint8_t status)
* @brief set sensor stauts
* @param [in]ch: ch num, [in]status: 0 connect,1:disconnect
* @return None.
*/
void fw_reg_set_sensor_status(uint16_t ch, uint8_t status)   // 设置sensor通断
{
    uint16_t *lpData;

    lpData = (uint16_t *)&Reg_SensorState[0];

    if(ch < FW_SAMPLE_RAWDATA_MAX)
    {
        if(status)
        {
            *lpData |= (uint16_t)BIT(ch);
        }
        else
        {
            *lpData &= ~(uint16_t)BIT(ch);
        }

    }
}


/**
* @fn void fw_reg_set_temperature(uint16_t temp_data)
* @brief set temperature reg
* @param [in]temp_data: temperature data
* @return None.
*/
void fw_reg_set_temperature(int16_t temp_data)   // 设置温度传感器寄存器数值
{
    int16_t *lpData;
    lpData = (int16_t *)&Reg_Temperature[0];
    *lpData = temp_data;

}

//--debug mode----------------------------------------------------------------------------------------------
/**
* @fn void DebugMode_RawDataOut(void)
* @brief Upload raw data. Modify Reg_DebugDataNew and Reg_DataReadyNew.
* @param None
* @return None
*/
void DebugMode_RawDataOut(void)                                             //准备原始rawdata数据
{
    unsigned char i = 0;
    int16_t *lpData;

    //    if(Reg_DataReadyNew[0] == 0)
    {
        lpData = (int16_t *)&Reg_DebugDataNew[0];

        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
        {
            *lpData = fw_sample_get_rawdata(i);
            lpData ++;
        }

        Reg_DataReadyNew[0] = FW_SAMPLE_RAWDATA_NUM * 2;
    }
}

/**
* @fn void DebugMode_AfeInfoCRC16(void)
* @brief Upload afe info. Modify Reg_DebugDataNew and Reg_DataReadyNew.
* @param None
* @return None
*/
void DebugMode_AFEInfo(void)                            //准备 afe信息数据
{
    uint8_t data_num;

    data_num = hal_afe_info_get(Reg_DebugDataNew);

    Reg_DataReadyNew[0] = data_num;
}

/**
* @fn void DebugMode_RawDataCRC16(void)
* @brief Upload raw data and support crc. Modify Reg_DebugDataNew and Reg_DataReadyNew.
* @param None
* @return None
*/
void DebugMode_RawDataCRC16(void)                       //准备原始rawdata数据，带crc16校验码
{
    unsigned char i = 0;
    int16_t *lpData;

    //    if(Reg_DataReadyNew[0] == 0)
    {
        lpData = (int16_t *)&Reg_DebugDataNew[0];

        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
        {
            *lpData = fw_sample_get_rawdata(i);
            lpData ++;
        }
        *lpData = fw_system_crc16(Reg_DebugDataNew, FW_SAMPLE_RAWDATA_NUM * 2);

        Reg_DataReadyNew[0] = (FW_SAMPLE_RAWDATA_NUM + 1) * 2;
    }
}

/**
* @fn void DebugMode_NoiseCalculate(void)
* @brief
* @param None
* @return None
*/
void DebugMode_NoiseCalculate(void)                     //准备noise 数据
{
    static char flag_start = 0;
    char flag_finish = 0;
    uint16_t length = 0;
    uint16_t crc_value;
    char i;

    int16_t noise_peak[FW_SAMPLE_RAWDATA_NUM];
    int32_t noise_dev_sum[FW_SAMPLE_RAWDATA_NUM];

    if(flag_start == 0)
    {
        if(Reg_DataReadyNew[0] == 2)
        {
            length = Reg_DebugDataNew[1];
            length <<= 8;
            length += Reg_DebugDataNew[0];

            fw_sample_noise_cal_en(length);     // 使能noise计算处理

            flag_start = 1;
        }

    }
    else
    {
        flag_finish = fw_sample_noise_get(noise_peak, noise_dev_sum, FW_SAMPLE_RAWDATA_NUM);    //获取noise数据

        if(flag_finish)
        {
            for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
            {
                Reg_DebugDataNew[i * 6] = (unsigned char)noise_peak[i];
                Reg_DebugDataNew[i * 6 + 1] = (unsigned char)(noise_peak[i] >> 8);
                Reg_DebugDataNew[i * 6 + 2] = (unsigned char)(noise_dev_sum[i]);
                Reg_DebugDataNew[i * 6 + 3] = (unsigned char)(noise_dev_sum[i] >> 8);
                Reg_DebugDataNew[i * 6 + 4] = (unsigned char)(noise_dev_sum[i] >> 16);
                Reg_DebugDataNew[i * 6 + 5] = (unsigned char)(noise_dev_sum[i] >> 24);
            }

            if(Reg_DebugModeNew[0] == DEBUG_MODE_INTERNAL_NOISE_CALCULATE)  // 0x12
            {
                for(i = 0; i < CH_NUM; i++)
                {
                    if(noise_peak[i] > 20)
                    {
                        break;
                    }
                }
                //				if(i<CH_NUM)
                //				{
                //					Reg_DebugDataNew[6 * CH_NUM] = 0;
                //				}
                //				else
                {
                    Reg_DebugDataNew[6 * CH_NUM] = 4;
                }
                crc_value = fw_system_crc16(Reg_DebugDataNew, CH_NUM * 6 + 1 );
                Reg_DebugDataNew[6 * CH_NUM + 1] = crc_value & 0xFF;
                Reg_DebugDataNew[6 * CH_NUM + 2] = (crc_value >> 8) & 0xFF;
                Reg_DataReadyNew[0] = CH_NUM * 6 + 3;
            }
            else
            {
                Reg_DataReadyNew[0] = FW_SAMPLE_RAWDATA_NUM * 6;
            }


            flag_start = 0;
        }
    }
}

/**
* @fn void DebugMode_ForceDataCRC16(void)
* @brief
* @param None
* @return None
*/
void DebugMode_ForceDataCRC16(void)             //准备forcedata，带crc16校验
{
    char i;
    int16_t *lpData;

    for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
    {
        lpData = (int16_t *)&Reg_DebugDataNew[2 * i];
        *lpData = fw_reg_app_forcedata[i];
    }

    lpData = (int16_t *)&Reg_DebugDataNew[FW_SAMPLE_RAWDATA_NUM * 2];
    *lpData = fw_system_crc16(Reg_DebugDataNew, FW_SAMPLE_RAWDATA_NUM * 2);

    Reg_DataReadyNew[0] = FW_SAMPLE_RAWDATA_NUM * 2 + 2;
}

/**
* @fn void DebugMode_WriteChannelCalData(void)
* @brief
* @param None
* @return None
*/
void DebugMode_WriteChannelCalData(void)                    //写通道校准系数
{
    uint32_t factor;
    uint16_t ch;
    uint8_t check_sum;

    if(Reg_DataReadyNew[0] == 6 || Reg_DataReadyNew[0] == 7)
    {
        ch = Reg_DebugDataNew[0] | (Reg_DebugDataNew[1] << 8);
        if(ch < FW_SAMPLE_RAWDATA_MAX)
        {
            factor = Reg_DebugDataNew[2] | (Reg_DebugDataNew[3] << 8) \
                     | (Reg_DebugDataNew[4] << 16) | (Reg_DebugDataNew[5] << 24);
            if(Reg_DataReadyNew[0] == 7)
            {
                check_sum = Reg_DebugDataNew[0] + Reg_DebugDataNew[1] +
                            Reg_DebugDataNew[2] + Reg_DebugDataNew[3] + Reg_DebugDataNew[4] + Reg_DebugDataNew[5];
                if(check_sum != Reg_DebugDataNew[6])
                {
                    Reg_DataReadyNew[0] = 1;

                    return;
                }
            }
            //			if(factor>100 && factor < 10000)
            {
                // 温度传感器修正处理
                g_factory_param.temperature_corr = TEMPERATURE_TARGET - (fw_system_get_temperature() - g_factory_param.temperature_corr);  //保存温度差值
                // limit
                if(ABS(g_factory_param.temperature_corr) > 2500 )       // 温度修正值太大，不使用
                {
                    g_factory_param.temperature_corr = 0;
                }
                g_factory_param.calibration_factor[ch] = factor;

                ft_set_factory_config(&g_factory_param);
                if(ft_get_factory_config(&g_factory_param) == 0)
                {
                    if(g_factory_param.calibration_factor[ch] != factor)
                    {
                            Reg_DataReadyNew[0] = 0;
     
                        return;
                    }

                }
            }

        }

    }

}

/**
* @fn void DebugMode_ReadChannelCalData(void)
* @brief
* @param None
* @return None
*/
void DebugMode_ReadChannelCalData(void)                     //读通道校准系数
{
    uint32_t factor;
    uint16_t ch;

    if(Reg_DataReadyNew[0] == 2)
    {
        ch = Reg_DebugDataNew[0] | (Reg_DebugDataNew[1] << 8);
        if(ch < FW_SAMPLE_RAWDATA_MAX)
        {
            factor = g_factory_param.calibration_factor[ch];
            Reg_DebugDataNew[0] = factor & 0xFF;
            Reg_DebugDataNew[1] = (factor >> 8) & 0xFF;
            Reg_DebugDataNew[2] = (factor >> 16) & 0xFF;
            Reg_DebugDataNew[3] = (factor >> 24) & 0xFF;
        }
    }

    Reg_DataReadyNew[0] = 4;   					//one factor is 4 bytes.
}

/**
* @fn void DebugMode_ReadForceThreshold(void)
* @brief Read different channel calibration factor from flash. Modify Reg_DebugDataNew and Reg_DataReadyNew.
* @param None
* @return None
*/
void DebugMode_ReadForceThreshold(void)
{

    if(Reg_DataReadyNew[0] == 2)
    {
        unsigned short ch = Reg_DebugDataNew[0];
        if(ch < 8)
        {
            unsigned short down_threshold = g_user_param.key_down_threshold[ch];
            unsigned short up_threshold = g_user_param.key_up_threshold[ch];
            Reg_DebugDataNew[0] = down_threshold & 0xFF;
            Reg_DebugDataNew[1] = (down_threshold >> 8) & 0xFF;
            Reg_DebugDataNew[2] = (up_threshold >> 0) & 0xFF;
            Reg_DebugDataNew[3] = (up_threshold >> 8) & 0xFF;
        }
    }

    Reg_DataReadyNew[0] = 4;    //one factor is 4 bytes.
}
/**
* @fn void DebugMode_WriteForceThreshold(void)
* @brief Read different channel calibration factor from flash. Modify Reg_DebugDataNew and Reg_DataReadyNew.
* @param None
* @return None
*/
void DebugMode_WriteForceThreshold(void)
{
	
    if(Reg_DataReadyNew[0] == 6)
    {
        unsigned short ch = Reg_DebugDataNew[0];
        if(ch < 8)
        {
            unsigned short down_threshold = Reg_DebugDataNew[2] | (Reg_DebugDataNew[3] << 8);
            unsigned short up_threshold = Reg_DebugDataNew[4] | (Reg_DebugDataNew[5] << 8);
            g_user_param.key_down_threshold[ch] = down_threshold;
            g_user_param.key_up_threshold[ch] = up_threshold;
            ft_set_user_config(&g_user_param);
            if(ft_get_user_config(&g_user_param) == 0)
            {
                if(g_user_param.key_down_threshold[ch] != down_threshold
                        || g_user_param.key_up_threshold[ch] != up_threshold )
                {
                    Reg_DataReadyNew[0] = 0;
                    return;
                }
				
				
				
            }
        }
    }
    Reg_DataReadyNew[0] = 1;
}

/**
* @fn DebugMode_IICDebugPrintf(void)
* @brief
* @param None
* @return None
*/
void DebugMode_IICDebugPrintf(void)                                  // 输出IIC printf
{
    unsigned char i = 0;
    uint16_t num;

    if(Reg_DataReadyNew[0] == 0)
    {
        num = fw_printf_iic_get_data_num();

        if(num > 255)
        {
            num = 255;
        }

        for(i = 0; i < num; i++)
        {
            Reg_DebugDataNew[i] = fw_printf_iic_get_data(i);
        }

        Reg_DataReadyNew[0] = num;
    }
}
/**
* @fn DebugMode_IICDebugPrintf(void)
* @brief
* @param None
* @return None
*/
void fw_reg_set_rawdata_full(int16_t *dat, uint8_t num)
{
    uint8_t i;
    int16_t checksum = 0;

    if(fw_reg_rawdata_full_set_count >= FW_REG_RAWDATA_FULL_MAX_FRAME)
    {
        fw_reg_rawdata_full_set_count = 0;
    }

    for(i=0;i<num;i++)
    {
        checksum += dat[i]; // 计算累加和

        fw_reg_rawdata_full_buffer[fw_reg_rawdata_full_set_count*(FW_SAMPLE_RAWDATA_NUM)+i] = dat[i]; // 填充rawdata
    }
    //fw_reg_rawdata_full_buffer[fw_reg_rawdata_full_set_count*(FW_SAMPLE_RAWDATA_NUM+1)+i] = checksum; // 填充累加和

    fw_reg_rawdata_full_set_count++; // 帧数计数
}

/**
* @fn DebugMode_IICDebugPrintf(void)
* @brief
* @param None
* @return None
*/
void DebugMode_RawdataFull_Output(char factor_flag)                                  // 输出IIC printf
{
    unsigned char i = 0;
    int16_t *lpData;
    uint16_t num;
    int16_t checksum = 0;
    int32_t temp_rawdata;

    if(Reg_DataReadyNew[0] == 0)
    {
        if(fw_reg_rawdata_full_set_count!=0)
        {
            lpData = (int16_t *)&Reg_DebugDataNew[0];

            num = fw_reg_rawdata_full_set_count*(FW_SAMPLE_RAWDATA_NUM);

            //填充数据
            for(i = 0; i < num; i++)
            {
                if(factor_flag==0) // 无校准系数
                {
                    *lpData = fw_reg_rawdata_full_buffer[i];
                }
                else
                {
                    temp_rawdata = fw_reg_rawdata_full_buffer[i];
                    temp_rawdata = temp_rawdata*fw_system_get_calibration_factor(i%FW_SAMPLE_RAWDATA_NUM) /1024;
                    *lpData = temp_rawdata;
                }
                checksum += *lpData;
                lpData++;
            }

            // 填充帧数
            *lpData = (int16_t)fw_reg_rawdata_full_set_count;
            lpData++;
            checksum += fw_reg_rawdata_full_set_count;

            // 填充校验和
            *lpData = checksum;

            // 设置ready 位
            Reg_DataReadyNew[0] = (num+2)*2;

            fw_reg_rawdata_full_set_count = 0;
        }
    }
}
/**
* @fn void DebugMode_AppRawDataForceDataOut(void)
* @brief Upload raw data and support crc. Modify Reg_DebugDataNew and Reg_DataReadyNew.
* @param None
* @return None
*/

void DebugMode_AppRawDataForceDataOut(void)                         // 准备app要输出的rawdata、forcedata、baseline数据
{
#if 1	
    unsigned char i = 0;
    int16_t *lpData;
    unsigned char checksum = 0;

    if(Reg_DataReadyNew[0] == 0)
    {
        for(i = 0; i <FW_TK_SAMPLE_RAWDATA_NUM; i++)                  // 准备app 的rawdata 数据
        {
            lpData = (int16_t *)&Reg_DebugDataNew[2 * i];
            *lpData = hal_tk_sample_get_rawdata(i);   //原始信号
        }
		for(i = 0; i <FW_SAMPLE_RAWDATA_NUM; i++)                  
        {
            lpData = (int16_t *)&Reg_DebugDataNew[2 * i + FW_TK_SAMPLE_RAWDATA_NUM*2];
            *lpData = hal_afe_get_rawdata(i);   //原始信号
        }

        for(i = 0; i < FW_TK_FT_RAWDATA_NUM; i++)
        {
            lpData = (int16_t *)&Reg_DebugDataNew[2 * i + FW_TK_FT_RAWDATA_NUM * 2];   // 准备app的baseline数据
            *lpData = 0;
        }


        for(i = 0; i <FW_TK_FT_RAWDATA_NUM; i++)
        {
            lpData = (int16_t *)&Reg_DebugDataNew[2 * i + FW_TK_FT_RAWDATA_NUM * 4];   // 准备app的forcedata数据
            *lpData = 0;
        }

        for(i = 0;i<FW_TK_FT_RAWDATA_NUM*6;i++)
        {
            checksum += Reg_DebugDataNew[i];
        }

        Reg_DebugDataNew[FW_TK_FT_RAWDATA_NUM*6] = checksum;

        Reg_DataReadyNew[0] =FW_TK_FT_RAWDATA_NUM*6+1;                        // 通知主控数据个数
    }
	
#else
    unsigned char i = 0;
    unsigned char checksum = 0;
    unsigned char count = 0;
    unsigned char temp_h, temp_l;
    
    //if(Reg_DataReadyNew[0] == 0)
    {
        for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)                  		        // 准备app 的rawdata 数据
        {            
            temp_l = hal_tk_sample_get_rawdata(i);
            temp_h = hal_tk_sample_get_rawdata(i)>>8;
            
            Reg_DebugDataNew[count++] = temp_l;
            Reg_DebugDataNew[count++] = temp_h;
            
			//fw_printf("cal rawdata[%d] = %d\r\n",i,fw_sample_get_rawdata(i));
        }
        
        
        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)                  		        // 准备app 的rawdata 数据
        {            
            temp_l = hal_afe_get_rawdata(i);
            temp_h = hal_afe_get_rawdata(i)>>8;
            
            Reg_DebugDataNew[count++] = temp_l;
            Reg_DebugDataNew[count++] = temp_h;
        }
		
        for(i = 0; i < count; i++)
        {
            checksum += Reg_DebugDataNew[i];
        }
        Reg_DebugDataNew[count] = checksum;

        Reg_DataReadyNew[0] = 9 * 2 + 1;                       // 通知主控数据个数
    
        
    }

#endif
}

/**
* @fn void DebugMode_CloseUploadEvent(void)
* @brief Disable event upload and key io change.
* @param None
* @return None
*/
void DebugMode_CloseUploadEvent(void)
{

    if(Reg_DataReadyNew[0] == 4)
    {
        unsigned short index = Reg_DebugDataNew[0] | (Reg_DebugDataNew[1] << 8);
        unsigned short value = Reg_DebugDataNew[2] | (Reg_DebugDataNew[3] << 8);
        if(index == 5)
        {
            if(value == 0)
            {
                //close upload event
                fw_notify_int_out_disable();
                fw_notify_key_out_disable();
                fw_reg_disable_time_count = fw_system_get_systick_time();
            }
            else
            {
                //open upload event
                fw_notify_int_out_enable();
                fw_notify_key_out_enable();
                fw_reg_disable_time_count = 0;
            }
        }
    }

    Reg_DataReadyNew[0] = 0;            //! set a new data other than 4
}

/**
* @fn void DebugMode_Process(void)
* @brief Parse debug mode from iic.
* @param None
* @return None
*/
void DebugMode_Process(void)                            //debug mode 处理函数
{

    if(Reg_DebugModeNew[0] != 0)
    {
        switch(Reg_DebugModeNew[0])
        {
        case DEBUG_MODE_RAWDATA_OUT:
            DebugMode_RawDataOut();
            break;
        case DEBUG_MODE_AFE_INFO:
            DebugMode_AFEInfo();
            break;
        case DEBUG_MODE_RAWDATA_CRC16:
            DebugMode_RawDataCRC16();
            break;
        case DEBUG_MODE_NOISE_CALCULATE:
            DebugMode_NoiseCalculate();
            break;
        case DEBUG_MODE_INTERNAL_NOISE_CALCULATE:
            DebugMode_NoiseCalculate();
            break;
        case DEBUG_MODE_FORCEDATA_CRC16:
            DebugMode_ForceDataCRC16();
            break;
        case DEBUG_MODE_WRITE_CHANNEL_CAL_DATA:		//写校准系数
            DebugMode_WriteChannelCalData();
            break;
        case DEBUG_MODE_READ_CHANNEL_CAL_DATA:		//读校准系数
            DebugMode_ReadChannelCalData();
            break;
        case DEBUG_MODE_WRITE_FORCE_THRESHOLD:		//写按键阈值
            DebugMode_WriteForceThreshold();
            break;
        case DEBUG_MODE_READ_FORCE_THRESHOLD:		//读按键阈值
            DebugMode_ReadForceThreshold();
            break;
        case DEBUG_MODE_IIC_DEBUG_PRINTF:
            DebugMode_IICDebugPrintf();
            break;
        case DEBUG_MODE_RAWDATA_FACTOR_FULL_OUTPUT:
            DebugMode_RawdataFull_Output(1);
            break;
        case DEBUG_MODE_RAWDATA_FULL_OUTPUT:
            DebugMode_RawdataFull_Output(0);
            break;
        case DEBUG_MODE_CLOSE_UPLOAD_EVENT:
            DebugMode_CloseUploadEvent();
            break;
        case DEBUG_MODE_RAWDATA_FORCEDATA_OUT:		//获取AD原始值
            DebugMode_AppRawDataForceDataOut();
            break;

        default:
            break;
        }
    }
}

/**
* @fn static void I2C_State_Init(void)
* @brief Initial iic register.
* @param None
* @return None.
*/
static void fw_reg_debug_mode(void)            // 5组debug mode
{
    Reg_DebugModeNew = Reg_DebugMode;
    Reg_DataReadyNew = Reg_DataReady;
    Reg_DebugDataNew = Reg_DebugData1;
    DebugMode_Process();

    Reg_DebugModeNew = Reg_DebugModeB;
    Reg_DataReadyNew = Reg_DataReadyB;
    Reg_DebugDataNew = Reg_DebugDataB;
    DebugMode_Process();

    Reg_DebugModeNew = Reg_DebugModeC;
    Reg_DataReadyNew = Reg_DataReadyC;
    Reg_DebugDataNew = Reg_DebugDataC;
    DebugMode_Process();

    Reg_DebugModeNew = Reg_DebugModeD;
    Reg_DataReadyNew = Reg_DataReadyD;
    Reg_DebugDataNew = Reg_DebugDataD;
    DebugMode_Process();

    Reg_DebugModeNew = Reg_DebugModeE;
    Reg_DataReadyNew = Reg_DataReadyE;
    Reg_DebugDataNew = Reg_DebugDataE;
    DebugMode_Process();
}

/**
* @fn void fw_reg_cmd_funciton(uint8_t addr)
* @brief reg cmd proc
* @param None
* @return None.
*/
void fw_reg_cmd_funciton(uint8_t addr)          // 寄存器实时命令处理函数，中断调用
{
    switch(addr)
    {
    case REG_RESET_MCU:                     // 软复位
        hal_common_ic_soft_reset();
        break;

    case REG_WAKE_UP:
        fw_pmu_set_sleep_cmd(0);            // 退出sleep mode
        break;

    case REG_SLEEP:
        fw_pmu_set_sleep_cmd(1);            // 允许进入sleep mode
        break;

    default:
        break;
    }
}

/**
* @fn static void fw_reg_sw_reg(void)
* @brief reg sw proc
* @param None
* @return None.
*/
static void fw_reg_sw_reg(void)             // 开关寄存器数据处理
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        if(Reg_SW_enable[0]&BIT(i))         // 查找使能的bit位
        {
            fun_sw_enable[i]();
            Reg_SW_enable[0] &= ~BIT(i);
        }

        if(Reg_SW_disable[0]&BIT(i))        // 查找关闭的bit位
        {
            fun_sw_disable[i]();
            Reg_SW_disable[0] &= ~BIT(i);
        }
    }
}

/**
* @fn void fw_reg_param_funciton(void)
* @brief reg param proc
* @param None
* @return None.
*/
static void fw_reg_param_funciton(void)            // 寄存器参数命令处理函数
{
    uint16_t temp;
    uint8_t flag;

    // reg addr 0x0a
    temp = Reg_TaskEnable[1];
    temp <<= 8;
    temp |= Reg_TaskEnable[0];
    fw_system_app_enable_set(temp);

    fw_reg_sw_reg();                                // 开关寄存器处理

    if(fw_reg_disable_time_count)                   // io输出超时恢复处理，兼容旧协议
    {
        flag = fw_system_get_iic_flag();

        if(flag)                                    // 有iic通讯，保持更新计时
        {
            fw_reg_disable_time_count = fw_system_get_systick_time();
        }

        if(fw_system_get_systick_time() > (fw_reg_disable_time_count + 5000))   // iic 无通讯超过5s，恢复IO输出
        {
            //open upload event
            fw_notify_int_out_enable();
            fw_notify_key_out_enable();
            fw_reg_disable_time_count = 0;
        }
    }

}

/**
* @fn static void fw_reg_sw_reg(void)
* @brief reg sw proc
* @param None
* @return None.
*/
uint8_t fw_reg_get_debug_mode(void)             // 开关寄存器数据处理
{
    return Reg_DebugModeNew[0];
}

/**
* @fn void fw_reg_proc(void)
* @brief reg system proc
* @param None
* @return None.
*/
void fw_reg_proc(void)                      // 寄存器系统处理函数
{
    fw_reg_param_funciton();

    fw_reg_debug_mode();
}

/**
  * @}
  */

/**
  * @}
  */
