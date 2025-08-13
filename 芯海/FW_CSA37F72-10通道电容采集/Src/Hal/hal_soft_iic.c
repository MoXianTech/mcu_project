/**
* @file                hal_soft_iic.c
* @brief
* @author							 
* @version             V0.1.0
* @date                2021.8.9
* @details
* @copyright           (C) COPYRIGHT 2021 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

//driver
#include "csa37f7x.h"
#include "csa37f7x_rcc.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_i2c.h"
#include "csa37f7x_syscfg.h"
#include "csa37f7x_misc.h"
#include "csa37f7x_gpio.h"
#include "csa37f7x_rcc.h"
//framework
#include "framework_debug.h"
#include "user_config.h"
//hal
#include "hal_soft_iic.h"
#include "hal_timer.h"

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

/* IIC的SCL脚  GPIO_Mode_Out_OD */
#define I2C_SCL_PIN 			GPIO_Pin_0
#define I2C_SCL_NUM 			GPIO_PIN_NUM0
/* IIC的SDA脚  GPIO_Mode_Out_OD */
#define I2C_SDA_PIN				GPIO_Pin_7
#define I2C_SDA_NUM 			GPIO_PIN_NUM7
/* IIC引脚的所处类别 */
#define I2C_GPIOX  				GPIO_GROUP_A


/* 启动i2c（主机调用--产生时钟信号的那个机器） */
bool I2C_Start(void);

/* 终止i2c（主机调用--产生时钟信号的那个机器）  */
void I2C_Stop(void);

/* 从器件接收到来自主机的8位数据后，返回，要求主机继续发送数据 */
void I2C_Ack(void);

/* 从器件接收到来自主机的8位数据后，返回，要求主机终止发送数据 */
void I2C_NoAck(void);

/* 主机检查从机的是否要求继续发送数据的命令 */
bool I2C_WaitAck(void);   //返回为:=1有ACK,=0无ACK
/* 通过i2c总线发送一个8位的数据 */
void I2C_SendByte(unsigned char SendByte); //数据从高位到低位//

/* 通过i2c总线接收一个8位的数据 */ 
unsigned char I2C_ReceiveByte(void);  		//数据从高位到低位//
static int count = 0;

#define I2C_SCL_H         GPIOA->PXn_DOUT[GPIO_PIN_NUM0] = 1;/*gpio_bit_write(GPIO_GROUP_A, GPIO_PIN_NUM0, 1);*/	\
													while(((GPIOA->PX_PIN & GPIO_PIN_0)?SET:RESET)/*gpio_input_data_bit_read(GPIO_GROUP_A,GPIO_PIN_0)*/ == 0) \
													{																										\
																count++;																			\
																if(count>=15000)                              \
																{			                                        \
																	   count = 0;                               \
																	   break ;                                  \
																}															             		\
													}																						
#define I2C_SCL_L         GPIOA->PXn_DOUT[GPIO_PIN_NUM0] = 0;//gpio_bit_write(GPIO_GROUP_A, GPIO_PIN_NUM0, 0)
#define I2C_SDA_H         GPIOA->PXn_DOUT[GPIO_PIN_NUM7] = 1;//gpio_bit_write(GPIO_GROUP_A, GPIO_PIN_NUM7, 1)
#define I2C_SDA_L         GPIOA->PXn_DOUT[GPIO_PIN_NUM7] = 0;//gpio_bit_write(GPIO_GROUP_A, GPIO_PIN_NUM7, 0)
#define I2C_SDA_READ      ((GPIOA->PX_PIN & GPIO_PIN_7)?SET:RESET)//gpio_input_data_bit_read(GPIO_GROUP_A,GPIO_PIN_7)
#define I2C_SCL_READ      ((GPIOA->PX_PIN & GPIO_PIN_0)?SET:RESET)//gpio_input_data_bit_read(GPIO_GROUP_A,GPIO_PIN_0)


/* 直接影响IIC的速度 数值越小速度越快 */

#define I2C_SPEED_400K (1)
#define I2C_SPEED_200K_READ (10)
#define I2C_SPEED_200K_WRITE (8)

#define I2C_SPEED_100K_WRITE (21)
#define I2C_SPEED_100K_READ (31)

#define I2C_SPEED_50K_WRITE (48)
#define I2C_SPEED_50K_READ (71)

#define I2C_SPEED_20K_WRITE (128)
#define I2C_SPEED_20K_READ (191)

#define I2C_DELAY_TIME		(I2C_SPEED_400K)
#define I2C_NAK_DELAY (10)
static unsigned char IIC_ReadDelay;
static unsigned char IIC_WriteDelay;

void GPIO_SetBits(uint8_t group, uint8_t pin_num)
{
	gpio_bit_write(group, pin_num, 1);
	
}

void GPIO_ResetBits(uint8_t group, uint8_t pin_num)
{
	gpio_bit_write(group, pin_num, 0);
	
}

static void Delay_us(unsigned char time)
{    
    while(time--)
    {
  
    }

}

void SetIIC_Speed(unsigned char speed)
{
    switch(speed)
    {
    case 0:
        IIC_ReadDelay=I2C_SPEED_20K_READ;
        IIC_WriteDelay=I2C_SPEED_20K_WRITE;    
        break;
    case 1:
        IIC_ReadDelay=I2C_SPEED_50K_READ;
        IIC_WriteDelay=I2C_SPEED_50K_WRITE;
        break;
    case 2:
        IIC_ReadDelay=I2C_SPEED_100K_READ;
        IIC_WriteDelay=I2C_SPEED_100K_WRITE;
        break;
    case 3:
        IIC_ReadDelay=I2C_SPEED_200K_READ;
        IIC_WriteDelay=I2C_SPEED_200K_WRITE;
        break;
    case 4:
        IIC_ReadDelay=I2C_SPEED_400K;
        IIC_WriteDelay=I2C_SPEED_400K;
        break;
    default:
        IIC_ReadDelay=I2C_SPEED_400K;
        IIC_WriteDelay=I2C_SPEED_400K;
            break;
    }
}

unsigned char GetIIC_Speed(void)
{
    if(I2C_SPEED_20K_READ==IIC_ReadDelay
        &&I2C_SPEED_20K_WRITE==IIC_WriteDelay)
    {
        return 0;
    }
    if(I2C_SPEED_50K_READ==IIC_ReadDelay
        &&I2C_SPEED_50K_WRITE==IIC_WriteDelay)
    {
        return 1;
    }
    if(I2C_SPEED_100K_READ==IIC_ReadDelay
        &&I2C_SPEED_100K_WRITE==IIC_WriteDelay)
    {
        return 2;
    }
    if(I2C_SPEED_200K_READ==IIC_ReadDelay
        &&I2C_SPEED_200K_WRITE==IIC_WriteDelay)
    {
        return 3;
    }
    if(I2C_SPEED_400K==IIC_ReadDelay
        &&I2C_SPEED_400K==IIC_WriteDelay)
    {
        return 4;
    }
    IIC_ReadDelay=I2C_SPEED_20K_READ;
    IIC_WriteDelay=I2C_SPEED_20K_WRITE;
    return 0;
    
}


void I2C_GPIO_Config(void)
{

	gpio_config_t ptr_config;

    rcc_ahb_periph_clock_enable_ctrl(RCC_AHBPeriph_GPIOA, ENABLE);              //使能GPIOA时钟
	
	ptr_config.mode = GPIO_MODE_OUT_OD;
	ptr_config.pin = GPIO_PIN_0 | GPIO_PIN_7;
	ptr_config.pull = GPIO_PULL_NO_PULL;
	gpio_init(GPIO_GROUP_A,&ptr_config);
	
	I2C_SDA_H;
	I2C_SCL_H;
	
}



void iic_soft_init(void)
{
    I2C_GPIO_Config();
	
    SetIIC_Speed(4);

}

int test_start(void)
{
	  //I2C_Start();
	  //I2C_SDA_H;
	  //Delay_us(IIC_WriteDelay);
	  GPIO_SetBits( I2C_GPIOX , I2C_SDA_NUM );
	  GPIO_SetBits( I2C_GPIOX , I2C_SCL_NUM );
	  Delay_us(IIC_WriteDelay);
	  GPIO_ResetBits( I2C_GPIOX , I2C_SDA_NUM );
	  Delay_us(IIC_WriteDelay);
	  GPIO_ResetBits( I2C_GPIOX , I2C_SCL_NUM );
	  Delay_us(IIC_WriteDelay);
      GPIO_SetBits( I2C_GPIOX , I2C_SDA_NUM );
      Delay_us(IIC_WriteDelay);
      Delay_us(IIC_WriteDelay);
      Delay_us(1);
    return 1;
//	  GPIO_SetBits( I2C_GPIOX , I2C_SDA_PIN );
//	  Delay_us(IIC_WriteDelay);
//	  GPIO_SetBits( I2C_GPIOX , I2C_SCL_PIN );
//	  
//      GPIO_ResetBits( GPIOB , GPIO_Pin_7 );  //SDA
//	  GPIO_ResetBits( GPIOB , GPIO_Pin_6 );	 //SCL
}

bool I2C_Start(void)
{
    I2C_SDA_H;
    I2C_SCL_H;
    Delay_us(IIC_WriteDelay);
    if (!I2C_SDA_READ)return FALSE; //SDA线为低电平则总线忙,退出
    I2C_SDA_L;
    Delay_us(IIC_WriteDelay);
    if (I2C_SDA_READ) return FALSE; //SDA线为高电平则总线出错,退出
    I2C_SDA_L;
    Delay_us(IIC_WriteDelay);
    return TRUE;
}

void I2C_Stop(void)
{
    I2C_SCL_L;
    Delay_us(IIC_WriteDelay);
    I2C_SDA_L;
    Delay_us(IIC_WriteDelay);
    I2C_SCL_H;
    Delay_us(IIC_WriteDelay);
    I2C_SDA_H;
    Delay_us(IIC_WriteDelay);
}

void I2C_Ack(void)
{
    I2C_SCL_L;
    Delay_us(IIC_WriteDelay);
    I2C_SDA_L;
    Delay_us(IIC_WriteDelay);
    I2C_SCL_H;
    Delay_us(IIC_WriteDelay);
    I2C_SCL_L;
    Delay_us(IIC_WriteDelay);
}

void I2C_NoAck(void)
{
    I2C_SCL_L;
    Delay_us(IIC_WriteDelay);
    I2C_SDA_H;
    Delay_us(IIC_WriteDelay);
    I2C_SCL_H;
    Delay_us(IIC_WriteDelay);
    I2C_SCL_L;
    Delay_us(IIC_WriteDelay);
}

bool I2C_WaitAck(void)   //返回为:=1有ACK,=0无ACK
{
    I2C_SCL_L;
    Delay_us(IIC_WriteDelay);
    I2C_SDA_H;
    Delay_us(IIC_WriteDelay);
    I2C_SCL_H;
    Delay_us(IIC_WriteDelay);
    
    if ( I2C_SDA_READ )
    {
        I2C_SCL_L;
        return FALSE;
    }

    I2C_SCL_L;
    return TRUE;
}

void I2C_SendByte(unsigned char SendByte) //数据从高位到低位//
{
    unsigned char i=8;
    unsigned char j;
    while (i--)
    {
        I2C_SCL_L;
        Delay_us(IIC_WriteDelay);
        if (SendByte&0x80)
		{
			I2C_SDA_H;
		}
        else
		{
            I2C_SDA_L;
		}
        SendByte<<=1;
        Delay_us(IIC_WriteDelay);
        I2C_SCL_H;
        
        Delay_us(IIC_WriteDelay);
//        j=250;
//        while(!I2C_SCL_READ)//把延时放后面，有利于速度提升
//        {
//            j--;
//            if(j==0)
//            {
//                break;
//            }
//            
//        }
    }
    I2C_SCL_L;
}

unsigned char I2C_ReceiveByte(void)  //数据从高位到低位//
{
    unsigned char i=8;
    unsigned char j;
    unsigned char ReceiveByte=0;

    I2C_SDA_H;
    while (i--)
    {
        ReceiveByte<<=1;
        I2C_SCL_L;
//        Delay_us(IIC_ReadDelay);
		Delay_us(5);
        I2C_SCL_H;
        
//        Delay_us(IIC_ReadDelay);
//        j=250;
//        while(!I2C_SCL_READ)
//        {
//            j--;
//            if(j==0)
//            {
//                break;
//            }
//            
//        }
        if (I2C_SDA_READ)
        {
            ReceiveByte|=0x01;
        }
    }
    I2C_SCL_L;
    return ReceiveByte;
}



uint8_t I2C_ReadData(uint8_t slaveAddress ,uint16_t regAddress,uint8_t regLen , uint8_t* pBuffer,uint16_t len)
{
    
    if (!I2C_Start())
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(slaveAddress & 0xFE);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 2;
    }
    if(regLen==2)
    {
        I2C_SendByte(regAddress>>8);
        if(!I2C_WaitAck())
        {
            I2C_Stop();
            return 3;
        }
    }
    I2C_SendByte(regAddress);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 4;
    }
//    Delay_us(IIC_WriteDelay);//写读切换加延时
	Delay_us(100);//ydz???    
    if(!I2C_Start())
    {
        I2C_Stop();
        return 5;
    }
    I2C_SendByte(slaveAddress | 0x01);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 6;
    }
    while(len--)
    {
        *pBuffer=I2C_ReceiveByte();
        if(len==0)
        {
            I2C_NoAck();
        }
        else
        {
            I2C_Ack();
        }
        pBuffer++;
    }
    
    I2C_Stop();
    return 0;
}

unsigned char hal_i2c_read_soft(unsigned char slaveAddress ,unsigned int regAddress,unsigned char regLen , unsigned char* pBuffer,unsigned int len)
{
    volatile unsigned char ret;
    ret=I2C_ReadData(slaveAddress ,regAddress,regLen, pBuffer,len);
	
    if(2==ret)//唤醒
    {
        hal_timer_delay_ms(I2C_NAK_DELAY);
        ret=I2C_ReadData(slaveAddress ,regAddress,regLen, pBuffer,len);
    }

    return ret;
}

uint8_t I2C_start_addr(unsigned char slaveAddress)
{
    
    if (!I2C_Start())
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(slaveAddress & 0xFE );
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 2;
    }
    return 0;
}

uint8_t I2C_nostop(uint8_t slaveAddress , uint8_t * data, uint16_t len)
{
    if (!I2C_Start())
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(slaveAddress & 0xFE );
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 2;
    }
    while(len--)
    {
        I2C_SendByte(*data);
        I2C_WaitAck();
        data++;
    }
    return 0;
}

uint8_t I2C_write_data(uint8_t slaveAddress , uint8_t * data, uint16_t len)
{
    uint8_t* pBuffer;
    if (!I2C_Start())
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(slaveAddress & 0xFE );
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 2;
    }
    while(len--)
    {
        I2C_SendByte(*data);
        I2C_WaitAck();
        data++;
    }
    I2C_Stop();
    
    if (!I2C_Start())
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(slaveAddress & 0xFE);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 2;
    }

    while(len--)
    {
        *pBuffer=I2C_ReceiveByte();
        if(len==0)
        {
            I2C_NoAck();
        }
        else
        {
            I2C_Ack();
        }
        pBuffer++;
    }
    
    I2C_Stop();
    return 0;
}

uint8_t I2C_abnormal_start(uint8_t slaveAddress , uint8_t * pWData, uint16_t wLen ,uint8_t * pRData, uint16_t rLen)
{
	if(wLen>0)
	{
	    if (!test_start ())
	    {
	        I2C_Stop();
	        return 1;
	    }
	    I2C_SendByte(slaveAddress & 0xFE);
	    if(!I2C_WaitAck())
	    {
	        I2C_Stop();
	        return 2;
	    }
	    while(wLen--)
	    {
	        I2C_SendByte(*pWData);
	        if(!I2C_WaitAck())
	        {
	            I2C_Stop();
	            return 3;
	        }
			pWData++;
	    }

	    Delay_us(IIC_WriteDelay);//写读切换加延时
	}
	if(rLen>0)
	{
		if(!I2C_Start())
		{
		    I2C_Stop();
		    return 5;
		}
		I2C_SendByte(slaveAddress | 0x01);
		if(!I2C_WaitAck())
		{
		    I2C_Stop();
		    return 6;
		}
		while(rLen--)
		{
		    *pRData=I2C_ReceiveByte();
		    if(rLen==0)
		    {
		        I2C_NoAck();
		    }
		    else
		    {
		        I2C_Ack();
		    }
		    pRData++;
		}
	}
    I2C_Stop();
	return 0;
}


uint8_t I2C_WriteRead(uint8_t slaveAddress , uint8_t * pWData, uint16_t wLen ,uint8_t * pRData, uint16_t rLen)
{
	if(wLen>0)
	{
	    if (!I2C_Start())
	    {
	        I2C_Stop();
	        return 1;
	    }
	    I2C_SendByte(slaveAddress & 0xFE);
	    if(!I2C_WaitAck())
	    {
	        I2C_Stop();
	        return 2;
	    }
	    while(wLen--)
	    {
	        I2C_SendByte(*pWData);
	        if(!I2C_WaitAck())
	        {
	            I2C_Stop();
	            return 3;
	        }
			pWData++;
	    }

	    Delay_us(IIC_WriteDelay);//写读切换加延时
	}
	if(rLen>0)
	{
		if(!I2C_Start())
		{
		    I2C_Stop();
		    return 5;
		}
		I2C_SendByte(slaveAddress | 0x01);
		if(!I2C_WaitAck())
		{
		    I2C_Stop();
		    return 6;
		}
		while(rLen--)
		{
		    *pRData=I2C_ReceiveByte();
		    if(rLen==0)
		    {
		        I2C_NoAck();
		    }
		    else
		    {
		        I2C_Ack();
		    }
		    pRData++;
		}
	}
    I2C_Stop();
	return 0;
}

uint8_t I2C_WriteData(uint8_t slaveAddress , uint16_t regAddress ,uint8_t regLen ,uint8_t * data, uint16_t len)
{
    
    if (!I2C_Start())
    {
        I2C_Stop();
        return 1;
    }
    I2C_SendByte(slaveAddress & 0xFE );
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return 2;
    }
    
    if(regLen==2)
    {
        I2C_SendByte(regAddress>>8);
        I2C_WaitAck();
    }

    I2C_SendByte(regAddress);
    I2C_WaitAck();
    while(len--)
    {
        I2C_SendByte(*data);
        I2C_WaitAck();
        data++;
    }
    I2C_Stop();
    return 0;
}

unsigned char hal_i2c_write_soft(unsigned char slaveAddress, unsigned int regAddress ,unsigned char regLen ,unsigned char *data, unsigned int len)
{
    unsigned char ret;
    ret=I2C_WriteData(slaveAddress,regAddress ,regLen ,data,len);
    if(2==ret)//唤醒
    {
        hal_timer_delay_ms(I2C_NAK_DELAY);
        ret=I2C_WriteData(slaveAddress,regAddress ,regLen ,data,len);
    }

    return ret;
}


uint8_t I2C_StateCheck(uint8_t slaveAddress)
{
    bool result;
    I2C_Start();
    I2C_SendByte(slaveAddress & 0xFE );
    result=I2C_WaitAck();
    I2C_Stop();
    if(result==FALSE)
    {
        return 0;
    }
    return 1;
}




