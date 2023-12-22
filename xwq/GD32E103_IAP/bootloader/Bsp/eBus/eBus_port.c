#include "eBus_port.h"
#include "eBus_core.h"

#include "eBus_mcuboot.h"
#include "mculib_fmc.h"
#include "mculib_usart0.h"
#include "mlib_simplefifo.h"


void eBus_delay_ms(uint32 ms);
void sys_restart(void);

#ifdef _EBUS_MCUBOOT_H_

EBUS_MCUBOOT_ERR_ENUM eBus_mcuboot_erasure(uint32 add_length);
EBUS_MCUBOOT_ERR_ENUM eBus_mcuboot_write(uint32 offset_add,uint32 length,uint8 *w_buff);
EBUS_MCUBOOT_ERR_ENUM eBus_mcuboot_read(uint32 offset_add,uint32 length,uint8 *r_buff);

#endif  //#ifdef _EBUS_MCUBOOT_H_




EBUS_STATE_STRUCT ebus_state_struct;

EBUS_FLOW_READ_PFUNC eBus_flow_read_pfunc=eBus_flow_read_func;  //回调函数关联，读取数据
EBUS_FLOW_WRITE_PFUNC eBus_flow_write_pfunc=eBus_flow_write_func; //回调函数关联，发送数据（响应）

EBUS_STRUCT eBus_pack;
//外部调用
void eBus_init(void)
{
    ebus_state_struct.sit=0;
    ebus_state_struct.step=0;
    ebus_state_struct.time=0;
    //配置包解析超时时间 毫秒
    ebus_state_struct.timeout=100;
}

//外部调用
void eBus_run(uint16 time_ms)
{
    EBUS_ERR_ENUM state;
		//读取串口接收的数据并判断
    state = EBUS_flow_2_struct(eBus_flow_read_pfunc,&ebus_state_struct,&eBus_pack,time_ms);
	if(state == EBUS_ERR_ENUM_NONE)  //EBUS_ERR_ENUM_NONE表示数据接收完毕且校验通过
    {
			switch(eBus_pack.ptype)
			{
					case EBUS_PACK_TYPE_APP_UPGRADE:  //0xF0 代表 APP升级包
							#ifdef _EBUS_MCUBOOT_H_
									eBus_mcuboot_run(   &eBus_pack, //数据域，
																			eBus_flow_write_pfunc, //串口发送
																			eBus_mcuboot_erasure,  //擦除flash
																			eBus_mcuboot_write,//写偏移地址，写长度，写buff指针
																			eBus_mcuboot_read,//偏移地址，读取长度，读buff指针
																			eBus_delay_ms,//延时函数
																			sys_restart); //复位函数
									break;
							#else
							EBUS_creat_flow_d1(eBus_flow_write_pfunc,EBUS_PACK_TYPE_APP_UPGRADE,0xFF);
							break;
							#endif
					
					case EBUS_PACK_TYPE_DEVICE_CONTROL:  //0x20	设备控制命令	
							if(eBus_pack.data[0]==1)
							{
									//重启
									sys_restart();
							}
							break;
					case 1:break;
					case 2:break;
					case 3:break;
			}
    }
}



void eBus_delay_ms(uint32 ms)
{
    uint32 time;
    for(;ms>0;)
    {
        for(time=0;time<10000;)
        {
            time++ ;
        }
        ms--;
    }
}
/*
数据流读写接口，需要用户实现
*/
uint8 eBus_flow_read_func(uint8 *value)
{
    return simplefifo_buff_pt_read(value);
}
//函数回调：发送响应
//移植时这里需要更改成对应的串口发送函数
uint8 eBus_flow_write_func(uint8 value)
{
    mculib_usart0_send_byte(value);
		return 0;
}

//芯片重启接口，需要用户实现
void sys_restart(void)
{
    __set_FAULTMASK(1);//close all interrupt
    NVIC_SystemReset();//reset
}


/*
程序升级功能接口，需要用户实现
*/
#ifdef _EBUS_MCUBOOT_H_

//计算升级包大小，擦除足够的页数
//add_length:数据包总长度
EBUS_MCUBOOT_ERR_ENUM eBus_mcuboot_erasure(uint32 add_length)
{   
    //STM32F330 每一个page是1K
    uint32 start_add=EBUS_MCUBOOT_APP_START_ADD;  //数据写入的起始flash地址
    uint32 ErasePageCounter;
    //计算需要擦除的页数
    uint32 PageNum = add_length / EBUS_MCUBOOT_PAGE_SIZE;
    //地址错误 必须为1k整数
    if(start_add%0x400!=0)return EBUS_MCUBOOT_ERR_ENUM_ADD_ERR;
    if(add_length%0x400!=0)
    {
        PageNum+=1;
    }
		//判断数据长度是否超过了flash内存大小
    if(add_length+start_add>EBUS_MCUBOOT_APP_START_ADD+EBUS_MCUBOOT_APP_SIZE)
    {
        return EBUS_MCUBOOT_ERR_ENUM_OVERLONG;
    }
    //
    fmc_unlock();
    fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
    for(ErasePageCounter = 0U; ErasePageCounter < PageNum; ErasePageCounter++)
    {
        fmc_page_erase(start_add + (ErasePageCounter<<10));
        fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
    }
    fmc_lock();
    return EBUS_MCUBOOT_ERR_ENUM_NONE;
}


//
EBUS_MCUBOOT_ERR_ENUM eBus_mcuboot_write(uint32 offset_add,uint32 length,uint8 *w_buff)
{
    uint32 address = offset_add+EBUS_MCUBOOT_APP_START_ADD;
    uint32 *add_pt=(uint32_t *)w_buff;
    uint16 i=0;
    
    fmc_unlock();
    while(address+4 <= offset_add+EBUS_MCUBOOT_APP_START_ADD+length)
    {
        fmc_word_program(address, *add_pt);
        address += 4U;
        add_pt+=1U;
        i+=4;
        fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
        if(i>=EBUS_MCUBOOT_APP_START_ADD+EBUS_MCUBOOT_APP_SIZE)
        {
            break;
        }
    }
    fmc_lock();
    return EBUS_MCUBOOT_ERR_ENUM_NONE;
}



//从对应偏移长度offset_add处读取length数量的数据，存到r_buff中
EBUS_MCUBOOT_ERR_ENUM eBus_mcuboot_read(uint32 offset_add,uint32 length,uint8 *r_buff)
{
    uint32 address = offset_add+EBUS_MCUBOOT_APP_START_ADD;
    uint8 *add_pt=(uint8_t *)address;
    uint16 i=0;
    for(i=0;i<length;i++)
    {
        r_buff[i]=add_pt[i];
    }
    return EBUS_MCUBOOT_ERR_ENUM_NONE;
}

#endif









