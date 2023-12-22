#include "ebus.h"
#include "uart.h"


EBUS_STATE_STRUCT ebus_state_struct;
EBUS_STRUCT eBus_pack;

//函数返回 EBUS_ERR_NONE 表示收到一个完整数据包
EBUS_ERR_ENUM  EBUS_flow_2_struct(//数据流转换为一个数据帧
EBUS_STATE_STRUCT *sate,    //数据帧转换状态（结构体指针）
EBUS_STRUCT *pack,          //数据帧指针 （结构体指针）
uint16 divtime_ms               //函数调用时间间隔（毫秒）
)
{
			uint8 data;
			while(USART1_Recv(&data, 1) != 0)
			{
					sate->time = 0;
					switch(sate->step)
					{
							case 0:
									if(data==EBUS_HEAD1)sate->step=10;//判断帧头
									break;
							case 10:
									if(data==EBUS_HEAD2)sate->step=20;//判断帧头
									else if(data!=EBUS_HEAD1)sate->step=0;
									break;
							case 20: //获取包类型
									pack->ptype=data;//获取帧类型
									pack->sumnot=data;//初始化校验值
									sate->sit=0;        //初始化数据位置
									sate->step=30;
									break;
							case 30://获取参数长度1
									pack->datalen=(data&0x7F);
							//当前参数长度字节bit7位为高时，表示后面一个字节也是表示参数长度。
									if((data&0x80)==0x80) 
									{
											sate->step=32;
									}
									else 
									{
											if(pack->datalen>EBUS_MAX_PACK_LENGTH)sate->step=0;//数据长度超过最大限制
											else sate->step=40;
									}
									pack->sumnot+=data;
									break;
							case 32: //获取参数长度2
									if((data&0x80)==0x80)
									{
											if(data==EBUS_HEAD1)sate->step=10;
											else sate->step=0;
									}
									else 
									{
											pack->datalen|=(data<<7);
											if(pack->datalen>EBUS_MAX_PACK_LENGTH) sate->step=0;//数据长度超过最大限制
											else if (pack->datalen == 0) sate->step = 50;
											else sate->step=40;
											
									}
									pack->sumnot+=data;
									break;
							case 40: //在这里接收data数据，直到接收完毕
									pack->data[sate->sit] = data;//获取帧数据
									pack->sumnot += data;
									sate->sit++;
									if(sate->sit>=pack->datalen)  //数据接收完毕
									{
											sate->step=50;
									}
									break;
									
							case 50:
									pack->sumnot=~pack->sumnot;
									sate->step=0;
									if(pack->sumnot==data)//判断校验
									{
											return EBUS_ERR_ENUM_NONE;  //校验成功，返回
									}
									break;
							default:
									if(data==EBUS_HEAD1)sate->step=10;
									else sate->step=0;
									break;
					}
			}
			if(sate->step!=0)
			{
					sate->time+=divtime_ms;//协议解析时间累计
					if(sate->time>sate->timeout)//数据流超时
					{
							sate->step=0;
							sate->time=0;
							return EBUS_ERR_ENUM_OVERTIME;
					}
			}
			return EBUS_ERR_ENUM_WAIT;
}

void eBus_mcuboot_run(EBUS_STRUCT *eBus_pack)
{
	switch(eBus_pack->data[0])
	{
			case EBUS_MCUBOOT_ENABLE: //0x70  进入BOOT模式
          __set_FAULTMASK(1);//close all interrupt
					NVIC_SystemReset();//reset
          break;
	}
}


void eBus_init(void)
{
    ebus_state_struct.sit=0;
    ebus_state_struct.step=0;
    ebus_state_struct.time=0;
    //配置包解析超时时间 毫秒
    ebus_state_struct.timeout=100;
}


//多少毫秒调用一次
void bootloader_check(uint16 time_ms)
{
		EBUS_ERR_ENUM state;
		//读取串口接收的数据并判断
		state = EBUS_flow_2_struct(&ebus_state_struct,&eBus_pack,time_ms);
		if(state == EBUS_ERR_ENUM_NONE)  //EBUS_ERR_ENUM_NONE表示数据接收完毕且校验通过
		{
			switch(eBus_pack.ptype)
			{
					case EBUS_PACK_TYPE_APP_UPGRADE:		//0xF0	代表 APP升级包
						eBus_mcuboot_run(&eBus_pack);
						break;
			}
		}
}
