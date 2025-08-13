
/****************************************************
* 文 件 名：gt9147.c
* 描    述：GT9147操作文件 - GD32版本
* 作    者：YouDream
* 修改者：根据要求适配GD32
* 编写时间：2015.02.04
* 修改时间：2023.11.15
* 说    明：适配GD32系列微控制器
            引脚配置：RST=PA2, INT=PA0, SCL=PA1, SDA=PA3
*****************************************************/
#include "gt9147.h"
#include "systick.h"
#include "gd32e10x.h"
#include "gd32e10x_gpio.h"
#include "uart.h"
/****************************************************
* GT9147配置信息表
* 第一个字节为版本号(0X60),必须保证新的版本号大于等于GT9147内部flash原有版本号,才会更新配置.
* 最后两个字节分别为写入"配置信息校验(0x8047到0x80FE之字节和的补码)和配置已更新标记(由主控写入标记0x01)"寄存器的值，
* 在写入配置信息表之时需要进行计算，预设值为0。
*****************************************************/



uint8_t GT9147_ConfigMSGTBL[186] = 
{ 
    0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
    0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
    0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
    0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
    0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
    0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
    0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
    0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
    0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
    0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
    0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
    0XFF,0XFF,0XFF,0XFF,0   ,0
};

volatile uint8_t RevAckF = 1;    //收到应答信号的标志位,为0表示收到
TouchPointRefTypeDef TPR_Structure = {0,{0},{0}};    //定义一个TouchPointRefTypeDef类型的结构体

TouchPointRefTypeDef TPR_Structure_t = {0,{0},{0}};    //定义一个TouchPointRefTypeDef类型的结构体


void check_touch(void){
	static char dma_buffer[256];  // 足够大的缓冲区
	int pos = 0;
	if(TPR_Structure.TouchSta >= 1){
		for (int i = 0; i < 5; i++) {
				
        if (TPR_Structure.x[i] != TPR_Structure_t.x[i] || TPR_Structure.y[i] != TPR_Structure_t.y[i]) {
//						pos += snprintf(dma_buffer + pos, sizeof(dma_buffer) - pos,
//							"CH[%d] x: %d ;y: %d\n", i, TPR_Structure.x[i], TPR_Structure.y[i]);
//					pos += snprintf(dma_buffer + pos, sizeof(dma_buffer) - pos,
//							"%d,%d\n",TPR_Structure.x[i], TPR_Structure.y[i]);
//						usart_dma_send_data(USART_0_TR,(uint8_t *)dma_buffer, pos);
//						printf("CH[%d] x: %d ;y: %d\n", i, TPR_Structure.x[i], TPR_Structure.y[i]);
            TPR_Structure_t.x[i] = TPR_Structure.x[i];
						TPR_Structure_t.y[i] = TPR_Structure.y[i];
					
        }
    }
		pos += snprintf(dma_buffer + pos, sizeof(dma_buffer) - pos,
							"%d,%d\n",TPR_Structure.x[0], TPR_Structure.y[0]);
						usart_dma_send_data(USART_0_TR,(uint8_t *)dma_buffer, pos);
	}
}

/**
 * @brief 将 PA3 设置为浮空输入模式（无上拉/下拉）
 */
void GT9147_SDA_SetAsIntput(void)
{
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_3);
}

/**
 * @brief 将 PA3 设置为推挽输出模式（默认无上拉/下拉）
 */
void GT9147_SDA_SetAsOutput(void)
{
    gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, GPIO_PIN_3);
}

/**
 * @brief 微秒级延时（基于 for 循环）
 * @param us  延时的微秒数
 */
void Delay_Xus(uint32_t us)
{
    /* 120MHz 下，粗略估算 1us ≈ 30 个循环 */
    uint32_t delay_cycles = us * 30;
    
    for (volatile uint32_t i = 0; i < delay_cycles; i++)
    {
        __NOP();  // 空指令，防止被编译器优化
    }
}

/****************************************************
* 函数名称：GT9147_Start
* 功    能：单片机发送起始信号
* 入口参数：无
* 返回参数：无
* 注意事项：
*****************************************************/
void GT9147_Start(void)                    
{
    GT9147_SDA_SetAsOutput();    //SDA口设置为输出
    GT9147_SDA_HIGH();                                    
    GT9147_SCL_HIGH();            //SCL最小高电平脉宽:0.6us
    Delay_Xus(2);                //起始信号的最小建立时间:0.6us
    GT9147_SDA_LOW();            //SCL高电平期间，SDA的一个下降沿表示起始信号
    Delay_Xus(2);                //起始信号的最小保持时间:0.6us
    GT9147_SCL_LOW();            //箝住总线,为发送器件地址做准备;
    Delay_Xus(2);                //SCL最小低电平脉宽:1.2us,由RET实现
}

/****************************************************
* 函数名称：GT9147_Stop
* 功    能：单片机发送停止信号
* 入口参数：无
* 返回参数：无
* 注意事项：
*****************************************************/
void GT9147_Stop(void)                            
{
    GT9147_SDA_SetAsOutput();    //SDA口设置为输出    
    GT9147_SDA_LOW();                                    
    GT9147_SCL_HIGH();            //SCL最小高电平脉宽:0.6us    
    Delay_Xus(1);                //停止信号的最小建立时间:0.6us
    GT9147_SDA_HIGH();            //SCL高电平期间，SDA的一个上升沿表示停止信号                                        
    Delay_Xus(2);                //新的发送开始前总线最小空闲时间:1.2us
    GT9147_SCL_LOW();            //箝住总线,为新的发送做准备;
    Delay_Xus(2);                //SCL最小低电平脉宽:1.2us,由RET实现
}

/****************************************************
* 函数名称：GT9147_McuACK
* 功    能：单片机发送应答信号
* 入口参数：无
* 返回参数：无
* 注意事项：单片机读1B数据后发送一个应答信号
*****************************************************/
void GT9147_McuACK(void)                            
{
    GT9147_SDA_SetAsOutput();    //SDA口设置为输出
    GT9147_SDA_LOW();                                                                        
    GT9147_SCL_HIGH();            //SCL最小高电平脉宽:0.6us
    Delay_Xus(1);
    GT9147_SCL_LOW();            //SCL最小低电平脉宽:1.2us
    Delay_Xus(2);
}

/****************************************************
* 函数名称：GT9147_McuNACK
* 功    能：单片机发送非应答信号
* 入口参数：无
* 返回参数：无
* 注意事项：单片机读数据停止前发送一个非应答信号
*****************************************************/
void GT9147_McuNACK(void)
{
    GT9147_SDA_SetAsOutput();    //SDA口设置为输出
    GT9147_SDA_HIGH();                                                                        
    GT9147_SCL_HIGH();            //SCL最小高电平脉宽:0.6us
    Delay_Xus(1);
    GT9147_SCL_LOW();            //SCL最小低电平脉宽:1.2us
    Delay_Xus(2);
}

/****************************************************
* 函数名称：GT9147_CheckAck
* 功    能：单片机检查GT9147送来的应答信号
* 入口参数：无
* 返回参数：无
* 注意事项：单片机写1个地址/数据后检查
             全局变量RevAckF:收到AT24C应答信号的标志位,为0表示收到
*****************************************************/
void GT9147_CheckAck(void)                            
{
    GT9147_SDA_SetAsIntput();    //置SDA为输入口
    GT9147_SCL_HIGH();            //使SDA上数据有效;SCL最小高电平脉宽:0.6us
    Delay_Xus(1);
    RevAckF = 0;                //预设单片机收到应答信号
    if (GT9147_SDA_Read())      //检查SDA状态,有应答则RevAckf=0
        RevAckF = 1;            //无应答则RevAckf=1
    GT9147_SCL_LOW();           //箝住总线,为新的发送做准备;
    Delay_Xus(2);               //SCL最小低电平脉宽:1.2us,由RET实现
}

/****************************************************
* 函数名称：GT9147_WrOneByte
* 功    能：单片机向IIC总线发送1B的地址/数据
* 入口参数：待发送的1B地址/数据
* 返回参数：无
* 注意事项：不是一个完整的数据发送过程;送数的顺序是从高到低
*****************************************************/
void GT9147_WrOneByte(uint8_t dat)                        
{
    uint8_t i;                    //地址/数据位数暂存单元    
    GT9147_SDA_SetAsOutput();    //SDA口设置为输出
    for(i = 8; i > 0; i--)        //8位1B地址/数据的长度
    {
        if(dat & 0x80)         
            GT9147_SDA_HIGH();    //发送"1"        
        else
            GT9147_SDA_LOW();    //发送"0"
				
				Delay_Xus(2);   
        GT9147_SCL_HIGH();        //使SDA上的数据有效
        Delay_Xus(2);            //SCL最小高电平脉宽:0.6us                            
        GT9147_SCL_LOW();        //SCL最小低电平脉宽:1.2us
        Delay_Xus(2);
        dat <<= 1;                //发送数据左移1位,为下位发送准备    
    } 
		
}

/****************************************************
* 函数名称：GT9147_RdOneByte
* 功    能：单片机从IIC总线接收1B的数据
* 入口参数：无
* 返回参数：收到的1B数据
* 注意事项：不是一个完整的数据接收过程;从高到低的顺序接收数据
*****************************************************/
uint8_t dat = 0; 
int a1223321 = 0;
uint8_t GT9147_RdOneByte(void)                        
{
    dat = 0;            //接收数据位数和内容暂存单元
    GT9147_SDA_SetAsIntput();    //置SDA为输入口
    for(int i = 8;i > 0;i--)
    {
        GT9147_SCL_HIGH();        //使SDA上的数据有效;SCL最小高电平脉宽:0.6us
        Delay_Xus(1);            //SCL最小高电平脉宽:0.6us
        dat <<= 1;                //数据dat左移1位,注意以下两句顺序不可颠倒
        if(GT9147_SDA_Read())    //读入SDA上的数据
            dat |= 0x01;
				a1223321 = GT9147_SDA_Read();
        GT9147_SCL_LOW();        //SCL为低电平允许SDA上的数据变化
        Delay_Xus(2);            //SCL最小低电平脉宽:1.2us
   }    
    return(dat);                //返回1B的数据
}

/****************************************************
* 函数名称：GT9147_WrNByte
* 功    能：单片机向GT9147发送n字节的数据
* 入口参数：写器件地址，寄存器首地址，发送数据缓冲区首地址,写数据的长度
* 返回参数：无
* 注意事项：
*****************************************************/
void GT9147_WrNByte(uint8_t Daddr,uint16_t Waddr,uint8_t a[],uint8_t n)
{
    uint8_t k;
    do
    {
        GT9147_Start();                            //发送起始信号
        GT9147_WrOneByte(Daddr & 0xfe);            //发送写器件地址
        GT9147_CheckAck();                        //检查应答位
        if(RevAckF) continue;                    //未收到应答位则重发
        GT9147_WrOneByte((uint8_t)(Waddr >> 8));//发送1B的高位地址
        GT9147_CheckAck();                        //检查应答位
        if(RevAckF) continue;                    //未收到应答位则重发
        GT9147_WrOneByte((uint8_t)(Waddr));        //发送1B的低位地址
        GT9147_CheckAck();                        //检查应答位    
        if(RevAckF) continue;                    //未收到应答位则重发        
        for (k=0; k<n; k++)                    
        {
            GT9147_WrOneByte(a[k]);                //发送1B的数据
            GT9147_CheckAck();                    //检查AT24Cxx送来的应答信号
            if(RevAckF) break;                    //未收到应答位说明发送错误了,需从头开始
        }    
    } while(RevAckF);
    GT9147_Stop();                                //发送停止信号
    delay_1ms(1);                                //延时1ms,延时等待EEPROM内部数据处理
}

/****************************************************
* 函数名称：GT9147_RdNByte
* 功    能：单片机从GT9147接收n字节的数据
* 入口参数：器件地址，寄存器首地址，接收数据缓冲区首地址,写数据的长度
* 返回参数：无
* 注意事项：
*****************************************************/
void GT9147_RdNByte(uint8_t Daddr,uint16_t Waddr,uint8_t a[],uint8_t n)
{
    uint8_t k;
    do
    {
        GT9147_Start();                            //发送起始信号
        GT9147_WrOneByte(Daddr & 0xfe);            //发送写器件地址
        GT9147_CheckAck();                        //检查应答位
        if(RevAckF) continue;                    //未收到应答位则重发
        GT9147_WrOneByte((uint8_t)(Waddr >> 8));//发送1B的高位地址
        GT9147_CheckAck();                        //检查应答位
        if(RevAckF) continue;                    //未收到应答位则重发
        GT9147_WrOneByte((uint8_t)(Waddr));        //发送1B的低位地址
        GT9147_CheckAck();                        //检查应答位    
        if(RevAckF) continue;                    //未收到应答位则重发
        GT9147_Start();                            //发送起始信号        
        GT9147_WrOneByte(Daddr | 0x01);            //发送读器件地址
        GT9147_CheckAck();                        //检查应答位
        if(RevAckF) continue;                    //未收到应答位则重发
    } while(RevAckF);      
    for(k = 0; k < n; k++)                    
    {
        a[k] = GT9147_RdOneByte();                //读入1B数据到接收数据缓冲区中
        GT9147_McuACK();                        //发送应答位
    }
    GT9147_McuNACK();                            //n个字节读完,发送非应答位
    GT9147_Stop();                                //发送停止信号
}

/****************************************************
* 函数名称：GT9147_SendConfigMSG
* 功    能：向GT9147写入配置信息
* 入口参数：cmd，为1更新配置信息，为0不更新
* 返回参数：None
* 注意事项：
*****************************************************/
void GT9147_SendConfigMSG(uint8_t cmd)
{
    uint8_t i;
    GT9147_ConfigMSGTBL[184] = 0;
    for(i = 0;i < 184;i++)
    {
        GT9147_ConfigMSGTBL[184] += GT9147_ConfigMSGTBL[i];        //计算校验和(0x8047到0x80FE之间字节和)
    }
    GT9147_ConfigMSGTBL[184] = ~GT9147_ConfigMSGTBL[184] + 1;    //配置信息校验(0x8047到0x80FE之间字节和的补码)
    GT9147_ConfigMSGTBL[185] = cmd;                                //配置已更新标记，写入0x01则保存更新配置
    GT9147_WrNByte(GT9147_DevAdr0,GT9147_ConfigMsgReg,GT9147_ConfigMSGTBL,186);    //写入配置信息（0x8047到0x8100）
}

/****************************************************
* 函数功能：GT9147控制端口配置
* 入口参数：无
* 返回参数：无
* 注意事项：
*****************************************************/
void GT9147_GPIO_Config(void)
{	
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    
    /* 配置PA1(SCL)为推挽输出 */
    gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, GPIO_PIN_1);
    gpio_bit_set(GPIOA,GPIO_PIN_1);
    /* 配置PA3(SDA)为开漏输出(需要上拉电阻) */
    gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, GPIO_PIN_3);
    gpio_bit_set(GPIOA,GPIO_PIN_3);
    /* 配置PA2(RST)为推挽输出 */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);
    
    /* 配置PA0(INT)为输入浮空 *///先拉低再配置浮空
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_0);
		gpio_bit_write(GPIOA, GPIO_PIN_0, RESET);  // SET = 高电平
}

void GT9147_Init(void)
{
    
    
    /* 初始化GPIO */
    GT9147_GPIO_Config();

    Delay_Xus(150);
    /* 硬件复位序列 */
		GT9147_RST_HIGH();      // 释放复位
    delay_1ms(6);

    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_0);//设置为浮空
		delay_1ms(50);

    GT9147_SendConfigMSG(0x01);
    
    /* 配置INT引脚为浮空输入 */
//    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    delay_1ms(80);
//		GT9147_WrOneByte(0xba);            //发送写器件地址
	
		static uint8_t id[4] = {0};
//		GT9147_RdNByte(GT9147_DevAdr0, 0x8140, id, 4); // 0x8140 是产品ID地址
		
//		printf("GT9147 ID: %c%c%c%c\r\n", id[0], id[1], id[2], id[3]);
}

const uint16_t GT9147_TPR_TBL[5] = {
    GT9147_TouchPoint1Reg,
    GT9147_TouchPoint2Reg,
    GT9147_TouchPoint3Reg,
    GT9147_TouchPoint4Reg,
    GT9147_TouchPoint5Reg
};

uint8_t sta = 0;
void GT9147_Scan(void)
{
    uint8_t i, dat[4] = {0};
    sta = 0;
    GT9147_RdNByte(GT9147_DevAdr0, GT9147_TouchStateReg, &sta, 1);
    
    if(sta & 0x80) {    // 坐标数据有效
        if(sta & 0x0F) { // 有触摸点按下
            TPR_Structure.TouchSta = ~(0xFF << (sta & 0x0F));
            
            for(i = 0; i < 5; i++) {
                if(TPR_Structure.TouchSta & (1 << i)) {
                    GT9147_RdNByte(GT9147_DevAdr0, GT9147_TPR_TBL[i], dat, 4);
                    
                    /* 横屏坐标转换 */
                    TPR_Structure.x[i] = (((uint16_t)(dat[3]) << 8) + dat[2]);
                    TPR_Structure.y[i] = ((uint16_t)(dat[1]) << 8) + dat[0];
                }
            }
        }
        
        /* 清除状态标志 */
        sta = 0;
        GT9147_WrNByte(GT9147_DevAdr0, GT9147_TouchStateReg, &sta, 1);
    } else {
        /* 清除所有触摸点标志 */
        TPR_Structure.TouchSta &= 0xE0;
    }
}
