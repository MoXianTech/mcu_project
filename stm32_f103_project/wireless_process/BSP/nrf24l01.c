/*************************************************************************
  > File Name: nrf24l01.c
  > Author: ma6174
  > Mail: ma6174@163.com
  > Created Time: 2022/7/15 17:27:42
 ************************************************************************/

#include "nrf24l01.h"

/*
 *  MISO-PA6
 *  MOSI-PA7
 *  SCK-PA5
 *  CSN-PA4
 *  CE-PA3
 *  IRQ-PA2
 */
#define TX_ADR_WIDTH    5     //5字节地址宽度
#define RX_ADR_WIDTH    5     //5字节地址宽度
#define TX_PLOAD_WIDTH  32    //32字节有效数据宽度
#define RX_PLOAD_WIDTH  32    //32字节有效数据宽度

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //发送地址


#define NRF_CSN PAout(4)
#define NRF_IRQ PAin(2)
#define NRF_SCK PAout(5)
#define NRF_CE PAout(3)
#define NRF_MOSI PAout(7)
#define NRF_MISO PAin(6)

void delay_us(uint8_t num)
{
    uint8_t i,j;
    for(i=0;i>num;i++)
        for(j=100;j>0;j--);
}
void delay_150us(void)
{
    uint16_t i;
    for(i=0;i>600;i++);
}

uint8_t SPI_RW(uint8_t byte)
{
    uint8_t bit_ctr;
    for(bit_ctr=0;bit_ctr<8;bit_ctr++)
    {
        if((uint8_t)(byte&0x80)==0x80)
            NRF_MOSI=1;
        else
            NRF_MOSI=0;
        byte=(byte<<1);
        NRF_SCK=1;
        byte|=NRF_MISO;
        NRF_SCK=0;
    }
    return byte;
}

uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
    uint8_t value;

    NRF_CSN=0;
    SPI_RW(reg);
    value = SPI_RW(NOP);
    NRF_CSN=1;
    return value;
}

uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
    uint8_t status;

    NRF_CSN=0;
    status = SPI_RW(reg);
    SPI_RW(value);
    NRF_CSN=1;

    return status;
}

uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    uint8_t status,u8_ctr;
    NRF_CSN=0;
    status = SPI_RW(reg);

    for(u8_ctr=0; u8_ctr<len; u8_ctr++)
        SPI_RW(*pBuf++);
    NRF_CSN=1;
    return status;
}

uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
    uint8_t status,u8_ctr;
    NRF_CSN=0;
    status=SPI_RW(reg);
    for(u8_ctr=0;u8_ctr<len;u8_ctr++)
        pBuf[u8_ctr]=SPI_RW(0XFF);
    NRF_CSN=1;
    return status;
}

bool nrf24l01_check(void)
{
    uint8_t check_in_buf[5]={0x11,0x22,0x33,0x44,0x55};
    uint8_t check_out_buf[5]={0x00};

    NRF_SCK=0;
    NRF_CSN=1;
    NRF_CE=0;

    NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR, check_in_buf, 5);

    NRF24L01_Read_Buf(nRF_READ_REG+TX_ADDR, check_out_buf, 5);

    if((check_out_buf[0] == 0x11)&&\
            (check_out_buf[1] == 0x22)&&\
            (check_out_buf[2] == 0x33)&&\
            (check_out_buf[3] == 0x44)&&\
            (check_out_buf[4] == 0x55))
        return 0;
    else
        return 1;
}

uint8_t nrf24l01_rx_packet(uint8_t *rxbuf)
{
    uint8_t state;
    state=NRF24L01_Read_Reg(STATUS);  			//读取状态寄存器的值    	 
    NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); //清除TX_DS或MAX_RT中断标志
    if(state & RX_OK)								//接收到数据
    {
        NRF_CE = 0;
        NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
        NRF24L01_Write_Reg(FLUSH_RX,0xff);					//清除RX FIFO寄存器
        NRF_CE = 1;
        delay_150us(); 
        return 0; 
    }	   
    return 1;//没收到任何数据
}
void nrf24l01_spi_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4
        | GPIO_Pin_5
        | GPIO_Pin_7
        | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2
        | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

uint8_t nrf24l01_tx_packet(uint8_t *txbuf)
{
	uint8_t state;
   
	NRF_CE=0;												//CE拉低，使能24L01配置
  	NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	//写数据到TX BUF  32个字节
 	NRF_CE=1;												//CE置高，使能发送	   
	while(NRF_IRQ==1);										//等待发送完成
	state=NRF24L01_Read_Reg(STATUS);  						//读取状态寄存器的值	   
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS, state); 			//清除TX_DS或MAX_RT中断标志
	if(state & MAX_TX)										//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);					//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(state & TX_OK)											//发送完成
	{
		return TX_OK;
	}
	return 0xff;											//发送失败
}

void nrf24l01_rt_init(void)
{	
    NRF_CE = 0;		  
    NRF24L01_Write_Reg(nRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度
    NRF24L01_Write_Reg(FLUSH_RX,0xff);									//清除RX FIFO寄存器    
    NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
    NRF24L01_Write_Buf(nRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
    NRF24L01_Write_Reg(nRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
    NRF24L01_Write_Reg(nRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
    NRF24L01_Write_Reg(nRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
    NRF24L01_Write_Reg(nRF_WRITE_REG+RF_CH,0);        //设置RF通道为2.400GHz  频率=2.4+0GHz
    NRF24L01_Write_Reg(nRF_WRITE_REG+RF_SETUP,0x0F);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
    NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0f);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
    NRF_CE=1;									  //CE置高，使能发送
}

void nrf24l01_send_buf(uint8_t *buf)
{
	NRF_CE=0;
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0e);
	NRF_CE=1;
	delay_us(15);
	nrf24l01_tx_packet(buf);
	NRF_CE=0;
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG, 0x0f);
	NRF_CE=1;	
}

void nrf24l01_init(void)
{
    nrf24l01_spi_init();
    return;
}

