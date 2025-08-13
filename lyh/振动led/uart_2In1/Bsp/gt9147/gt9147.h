/****************************************************
* 文 件 名：gt9147.h
* 描    述：GT9147操作文件 - GD32版本
* 作    者：YouDream
* 修改者：根据要求适配GD32
* 编写时间：2015.02.04
* 修改时间：2023.11.15
* 说    明：适配GD32系列微控制器
            引脚配置：RST=PA2, INT=PA0, SCL=PA1, SDA=PA3
*****************************************************/
#ifndef _GT9147_H
#define _GT9147_H

/*********************头文件包含*********************/
#include "gd32e10x.h"
#include "systick.h"



/*********************IO操作函数*********************/                                  
#define GT9147_RST_HIGH()          gpio_bit_set(GPIOA, GPIO_PIN_2)      // RST高电平
#define GT9147_RST_LOW()           gpio_bit_reset(GPIOA, GPIO_PIN_2)    // RST低电平
                                    
#define GT9147_SCL_HIGH()          gpio_bit_set(GPIOA, GPIO_PIN_1)      // SCL高电平
#define GT9147_SCL_LOW()           gpio_bit_reset(GPIOA, GPIO_PIN_1)    // SCL低电平
                                    
#define GT9147_SDA_HIGH()          gpio_bit_set(GPIOA, GPIO_PIN_3)      // SDA高电平
#define GT9147_SDA_LOW()           gpio_bit_reset(GPIOA, GPIO_PIN_3)    // SDA低电平
                                    
#define GT9147_SDA_Read()          gpio_input_bit_get(GPIOA, GPIO_PIN_3) // 读取SDA状态
                                    
#define GT9147_INT_Read()          gpio_input_bit_get(GPIOA, GPIO_PIN_0) // 读取INT状态

/***************GT9147 Device Address****************/
#define GT9147_DevAdr0             0xBA            //最低位是读写控制位,按位或0x01则读，按位与0xfe则写                              
/*                                  
#define GT9147_DevAdr1             0x28            //最低位是读写控制位,按位或0x01则读，按位与0xfe则写                                  
*/

/******************GT9147寄存器地址*******************/                                  
#define GT9147_ConfigMsgReg        0x8047          //GT9147配置信息寄存器起始地址
#define GT9147_ProductIDReg        0x8140          //GT9147产品ID寄存器地址    
#define GT9147_TouchStateReg       0X814E          //GT9147当前触摸情况寄存器地址                                  
#define GT9147_TouchPoint1Reg      0X8150          //GT9147触摸点1寄存器起始地址
#define GT9147_TouchPoint2Reg      0X8158          //GT9147触摸点2寄存器起始地址
#define GT9147_TouchPoint3Reg      0X8160          //GT9147触摸点3寄存器起始地址
#define GT9147_TouchPoint4Reg      0X8168          //GT9147触摸点4寄存器起始地址
#define GT9147_TouchPoint5Reg      0X8170          //GT9147触摸点5寄存器起始地址

//触摸点相关数据结构体定义
typedef struct          
{
    uint8_t TouchSta;   //触摸情况，bit7-bit5:保留；bit4-bit0触摸点按下有效标志，有效为1，分别对应触摸点5-1；
    uint16_t x[5];      //支持5点触摸，需要使用5组坐标存储触摸点数据
    uint16_t y[5];  
    
}TouchPointRefTypeDef;

//声明一个TouchPointRefTypeDef类型的结构体
extern TouchPointRefTypeDef TPR_Structure;

/********************外部函数声明********************/
extern void GT9147_WrNByte(uint8_t Daddr, uint16_t Waddr, uint8_t a[], uint8_t n);
extern void GT9147_RdNByte(uint8_t Daddr, uint16_t Waddr, uint8_t a[], uint8_t n);
extern void GT9147_SendConfigMSG(uint8_t cmd);
extern void GT9147_Init(void);
extern void GT9147_Scan(void);
void GT9147_WrOneByte(uint8_t dat);
void GT9147_Start(void);
void check_touch(void);                     
#endif

/*********************End of File********************/