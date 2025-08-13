/**
* @file                framework_reg.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef FRAMEWORK_REG_H
#define FRAMEWORK_REG_H

#include <stdint.h>                   /* Include standard types */
#include <string.h>

#define  MANUFACTURER_ID_MSB    0xC3
#define  MANUFACTURER_ID_LSB    0x72
#define  MODULE_ID_MSB          0x00
#define  MODULE_ID_LSB          0x72
#define  FW_VERSION_MSB	        0x00
#define  FW_VERSION_LSB         0x01

//--reg----------------------------------------------------
#define  REG_NOP                0x00
#define  REG_RESET_MCU          0x01
#define  REG_DEVICE_ID          0x02
#define  REG_MANUFACTURER_ID    0x03
#define  REG_MODULE_ID          0x04
#define  REG_FW_VERSION         0x05
#define  REG_WAKE_UP            0x06
#define  REG_SLEEP              0x07
#define  REG_RESET_PGA          0x09
#define  REG_TASK_ENABLE        0x0A
#define  REG_SW_ENABLE          0x0E
#define  REG_SW_DISABLE         0x0F

#define  REG_SENSOR_STATE       0x4E
#define  REG_IO_STATE           0x4F
#define  REG_HOST_STATUS        0x50
#define  REG_BATTERY_VOLTAGE    0x51
#define  REG_TEMPERATURE        0x52
#define  REG_DEBUG_MODE         0x60
#define  REG_DATA_READY         0x61
#define  REG_DEBUG_DATA1        0x62
#define  REG_DEBUG_DATA2        0x63
#define  REG_DEBUG_DATA3        0x64
#define  REG_DEBUG_DATA4        0x65
#define  REG_DEBUG_DATA5        0x66
#define  REG_DEBUG_DATA6        0x67
#define  REG_DEBUG_DATA7        0x68
#define  REG_DEBUG_DATA8        0x69
#define  REG_DEBUG_DATA9        0x6A
#define  REG_DEBUG_DATA10       0x6B
#define  REG_DEBUG_DATA11       0x6C
#define  REG_DEBUG_DATA12       0x6D
#define  REG_DEBUG_DATA13       0x6E
#define  REG_DEBUG_DATA14       0x6F
#define  REG_DEBUG_DATA15       0x70
#define  REG_DEBUG_DATA16       0x71
#define  REG_DEBUG_DATA17       0x72
#define  REG_DEBUG_DATA18       0x73
#define  REG_DEBUG_DATA19       0x74
#define  REG_DEBUG_DATA20       0x75
#define  REG_DEBUG_DATA21       0x76
#define  REG_DEBUG_DATA22       0x77
#define  REG_DEBUG_DATA23       0x78
#define  REG_DEBUG_DATA24       0x79
#define  REG_DEBUG_DATA25       0x7A
#define  REG_DEBUG_DATA26       0x7B
#define  REG_DEBUG_DATA27       0x7C
#define  REG_DEBUG_DATA28       0x7D
#define  REG_DEBUG_DATA29       0x7E
#define  REG_DEBUG_DATA30       0x7F
#define  REG_DEBUG_MODE_B       0x80
#define  REG_DATA_READY_B       0x81
#define  REG_DEBUG_DATA_B       0x82
#define  REG_DEBUG_MODE_C       0x83
#define  REG_DATA_READY_C       0x84
#define  REG_DEBUG_DATA_C       0x85
#define  REG_DEBUG_MODE_D       0x86
#define  REG_DATA_READY_D       0x87
#define  REG_DEBUG_DATA_D       0x88
#define  REG_DEBUG_MODE_E       0x89
#define  REG_DATA_READY_E       0x8A
#define  REG_DEBUG_DATA_E       0x8B
#define  REG_POINTER_NUMBER     0x10
#define  REG_POINTER_DATA1      0x11
#define  REG_POINTER_DATA2      0x12
#define  REG_POINTER_DATA3      0x13
#define  REG_POINTER_DATA4      0x14
#define  REG_POINTER_DATA5      0x15
#define  REG_POINTER_DATA6      0x16
#define  REG_POINTER_DATA7      0x17
#define  REG_POINTER_DATA8      0x18
#define  REG_POINTER_DATA9      0x19
#define  REG_POINTER_DATA10     0x1A

#define REG_TABLE_NUM           (0xFF)

#define  REG_RD_TEST            0x1F
#define  REG_FORCEDATA_NUMBER   0x20
#define  REG_FORCE_DATA1        0x21
#define  REG_FORCE_DATA2        0x22
#define  REG_FORCE_DATA3        0x23
#define  REG_FORCE_DATA4        0x24
#define  REG_FORCE_DATA5        0x25
#define  REG_FORCE_DATA6        0x26
#define  REG_FORCE_DATA7        0x27
#define  REG_FORCE_DATA8        0x28
#define  REG_FORCE_DATA9        0x29
#define  REG_FORCE_DATA10       0x2A

#define  REG_SW_IOSTATE         0x2B
#define  REG_GPIO_DISABLE       0x2C
#define  REG_UART_ENABLE        0x2D
#define  REG_IIC_PRINTF_ENABLE  0x2E

#define  REG_KEY_SENSITIVITY    0xD0
#define  REG_SELF_TEST          0xD1
#define  REG_KEY_SIMULATE       0xD2
#define  REG_KEY_OUTPUT         0xD3
#define  REG_UART_PRINT_ENABLE  0xD4


#define  REG_OP_COMMAND         0x00
#define  REG_OP_READ_ONLY       0x01
#define  REG_OP_WRITE_ONLY      0x10
#define  REG_OP_READ_WRITE      0x11


typedef struct
{
    uint16_t Cmd_Id;
    uint8_t Cmd_Type;
    uint8_t Cmd_Buf_Size;
    uint8_t *p_Cmd_Buf;
} TS_I2C_REG;

typedef enum
{
    REG_SW_INT_OUT = 0,
    REG_SW_KEY_IO = 1,
    REG_SW_MOTOR = 2,

    REG_SW_IIC_PRINT = 5,
    REG_SW_UART_PRINT = 6,
    REG_SW_SWD = 7,
} REG_SW_TypeDef;

//--debug mode--------------------------------------------------
#define DEBUG_MODE_OFF						0x00  //turn off debug mode
#define DEBUG_MODE_RAWDATA_OUT				0x01
#define DEBUG_MODE_RAWDATA_AND_TOUCHFLAG	0x02  //touch/leave/timeout/workmode/ flag
#define DEBUG_MODE_RAWDATA_SUMMARY			0x03  //force touch, rawdata summary
#define DEBUG_MODE_AFE_INFO					0x04
#define DEBUG_MODE_ADJUST_PARAMETER			0x05 //校准参数
#define DEBUG_MODE_RAED_PARAMETER			0x06 //读取参数
#define DEBUG_MODE_WRITE_DAC_DATA			0x07 //存储DAC
#define DEBUG_MODE_DIRECTLY_PARAMETER		0x08//直接写校准参数
#define DEBUG_MODE_READ_MID_ARG				0x09//读一些计算过程中的中间变量
#define DEBUG_MODE_WRITE_CAL_DATA_CHANNEL	0x0A//写每个通道校准数据
#define DEBUG_MODE_READ_CAL_DATA_CHANNEL	0x0B//读每个通道校准数据
#define DEBUG_MODE_RAWDATA_CRC16			0x10 //raw data with crc16
#define DEBUG_MODE_NOISE_CALCULATE			0x11 //calculate noise
#define DEBUG_MODE_INTERNAL_NOISE_CALCULATE	0x12 //calculate internal noise
#define DEBUG_MODE_SW_OPERATION				0x13  // 操作SW调试模式下的两个IO口
#define DEBUG_MODE_FORCEDATA_CRC16			0x20
#define DEBUG_MODE_WRITE_CHANNEL_CAL_DATA	0x30
#define DEBUG_MODE_READ_CHANNEL_CAL_DATA	0x31
#define DEBUG_MODE_WRITE_FORCE_THRESHOLD	0x32
#define DEBUG_MODE_READ_FORCE_THRESHOLD		0x33

#define DEBUG_MODE_IIC_DEBUG_PRINTF  		0x40
#define DEBUG_MODE_RAWDATA_FACTOR_FULL_OUTPUT  	0x41
#define DEBUG_MODE_RAWDATA_FULL_OUTPUT  	0x42
#define DEBUG_MODE_CLOSE_UPLOAD_EVENT		0xA1
#define DEBUG_MODE_RAWDATA_FORCEDATA_OUT	0xF4
//--------------------------------------------------------------

extern const TS_I2C_REG REG_TABLE[];
extern volatile unsigned char Sleep_Flag ;
extern volatile unsigned char WakeUp_Flag ;
extern volatile unsigned char Sensitivity_Flag;

extern unsigned char Register;

extern void I2C_Reg_Init(void);
extern void I2C_Reg_Function(uint16_t Cmd_Id);

extern unsigned char Reg_DeviceID[];
extern unsigned char Reg_ManufacturerID[];
extern unsigned char Reg_ModuleID[];
extern unsigned char Reg_FWVersion[];
extern unsigned char Reg_TaskEnable[];
extern unsigned char Reg_SensorState[];
extern unsigned char Reg_IoState[];

extern unsigned char Reg_DebugMode[];
extern unsigned char Reg_DataReady[];
extern unsigned char Reg_DebugData1[];
extern unsigned char Reg_DebugData2[];
extern unsigned char Reg_DebugData3[];
extern unsigned char Reg_DebugData4[];
extern unsigned char Reg_DebugData5[];
extern unsigned char Reg_DebugData6[];
extern unsigned char Reg_DebugData7[];
extern unsigned char Reg_DebugData8[];
extern unsigned char Reg_DebugData9[];
extern unsigned char Reg_DebugData10[];
extern unsigned char Reg_DebugData11[];
extern unsigned char Reg_DebugData12[];
extern unsigned char Reg_DebugData13[];
extern unsigned char Reg_DebugData14[];
extern unsigned char Reg_DebugData15[];
extern unsigned char Reg_DebugData16[];
extern unsigned char Reg_DebugData17[];
extern unsigned char Reg_DebugData18[];
extern unsigned char Reg_DebugData19[];
extern unsigned char Reg_DebugData20[];
extern unsigned char Reg_DebugData21[];
extern unsigned char Reg_DebugData22[];
extern unsigned char Reg_DebugData23[];
extern unsigned char Reg_DebugData24[];
extern unsigned char Reg_DebugData25[];
extern unsigned char Reg_DebugData26[];
extern unsigned char Reg_DebugData27[];
extern unsigned char Reg_DebugData28[];
extern unsigned char Reg_DebugData29[];
extern unsigned char Reg_DebugData30[];
extern unsigned char Reg_DebugModeB[];
extern unsigned char Reg_DataReadyB[];
extern unsigned char Reg_DebugDataB[];
extern unsigned char Reg_DebugModeC[];
extern unsigned char Reg_DataReadyC[];
extern unsigned char Reg_DebugDataC[];
extern unsigned char Reg_DebugModeD[];
extern unsigned char Reg_DataReadyD[];
extern unsigned char Reg_DebugDataD[];
extern unsigned char Reg_DebugModeE[];
extern unsigned char Reg_DataReadyE[];
extern unsigned char Reg_DebugDataE[];


extern unsigned char Reg_PointerNumber[];
extern unsigned char Reg_PointerData1[];
extern unsigned char Reg_PointerData2[];
extern unsigned char Reg_PointerData3[];
extern unsigned char Reg_PointerData4[];
extern unsigned char Reg_PointerData5[];
extern unsigned char Reg_PointerData6[];
extern unsigned char Reg_PointerData7[];
extern unsigned char Reg_PointerData8[];
extern unsigned char Reg_PointerData9[];
extern unsigned char Reg_PointerData10[];

extern unsigned char Reg_ForceDataNumber[];
extern unsigned char Reg_ForceData1[];
extern unsigned char Reg_ForceData2[];
extern unsigned char Reg_ForceData3[];
extern unsigned char Reg_ForceData4[];
extern unsigned char Reg_ForceData5[];
extern unsigned char Reg_ForceData6[];
extern unsigned char Reg_ForceData7[];
extern unsigned char Reg_ForceData8[];
extern unsigned char Reg_ForceData9[];
extern unsigned char Reg_ForceData10[];

extern unsigned char Reg_HostStatus[];
extern unsigned char Reg_BatteryVoltage[];
extern unsigned char Reg_Temperature[];
extern unsigned char Reg_KeySensitivity[];
extern unsigned char Reg_KeySimulate[];
extern unsigned char Reg_KeyOutput[];
extern unsigned char Reg_UARTPrintEnable[];

extern unsigned char Reg_WRBuff[];
extern unsigned char Reg_SWState[];
extern uint8_t Reg_GPIO_Disable[];
extern uint8_t Reg_UART_Enable[];
extern uint8_t Reg_IIC_Printf_Enable[];

extern unsigned char Reg_RDtest[];

extern unsigned char *Reg_DebugModeNew;
extern unsigned char *Reg_DataReadyNew;
extern unsigned char *Reg_DebugDataNew;

extern unsigned char Reg_SW_enable[];
extern unsigned char Reg_SW_disable[];

//--iic------------------------------------------
#define YES    (1)
#define NO     (0)

extern uint8_t I2C_AddrFlag;
extern uint8_t I2C_wPointer;
extern uint8_t I2C_rPointer;

void fw_reg_init(void);                                             // fw内部使用：reg模块初始化

void fw_reg_set(uint8_t addr, uint8_t offset, uint8_t dat);         // ap可使用：设置指定寄存器数值
uint8_t fw_reg_get(uint8_t addr, uint8_t offset);                   // ap可使用：读取指定寄存器数值
void fw_reg_set_word(uint8_t addr, uint8_t offset, uint16_t dat);   // ap可使用：设置指定寄存器数值
uint16_t fw_reg_get_word(uint8_t addr, uint8_t offset);             // ap可使用：设置指定寄存器数值
void fw_reg_set_press_reg(uint8_t key_num, uint16_t press_data);    // ap可使用：设置指定压力寄存器数值
void fw_reg_clear_press_reg(uint8_t key_num);                       // ap可使用：清除指定压力寄存器数值
void fw_reg_clear_all_press_reg(void);                              // ap可使用：清除所有压力寄存器数值
uint8_t fw_reg_get_point_num(void) ;                                // ap可使用：获取主控写入的坐标点个数
void fw_reg_get_point_coord(uint8_t point_num, uint16_t *x_coord, uint16_t *y_coord, uint8_t *press_status);    // ap可使用：获取主控写入的坐标点xy轴信息
uint8_t fw_reg_set_app_rawdata(uint8_t num, int16_t dat);           // ap可使用：设置ap指定的rawdata数值 （debug mode 0xf4）
uint8_t fw_reg_set_app_forcedata(uint8_t num, int16_t dat);         // ap可使用：设置ap指定的forcedata数值 （debug mode 0xf4）
uint8_t fw_reg_set_app_baseline(uint8_t num, int16_t dat);          // ap可使用：设置ap指定的baseline数值 （debug mode 0xf4）
void fw_reg_set_rawdata_full(int16_t *dat, uint8_t num);
void fw_reg_set_sensor_status(uint16_t ch, uint8_t status);         // fw内部使用：设置sensor通道寄存器
void fw_reg_set_temperature(int16_t temp_data);                     // fw内部使用：设置温度寄存器数值
void fw_reg_cmd_funciton(uint8_t addr);                             // fw内部使用：iic实时命令处理函数
uint8_t fw_reg_get_debug_mode(void);                           // fw内部使用：获取当前debug mode号
void fw_reg_proc(void);                                             // fw内部使用：reg模块处理函数

#endif

