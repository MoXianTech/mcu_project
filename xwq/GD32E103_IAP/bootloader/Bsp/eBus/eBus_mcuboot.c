#include "eBus_mcuboot.h"
#include "eBus_core.h"
#include "gd32e10x_fmc.h"

EBUS_MCUBOOT_STATE_ENUM eBus_mcuboot_state_enum=EBUS_MCUBOOT_STATE_ENUM_DISABLE;
EBUS_FLASH_PARAM_STRUCT eBus_flash_param_struct = {.update_flag = (uint32 *)UPDATE_FLAG_ADD};

void eBus_mcuboot_run(
    EBUS_STRUCT *eBus_pack,
    EBUS_FLOW_WRITE_PFUNC write,  //串口发送
    EBUS_MCUBOOT_ERR_ENUM (*rom_erasure)(uint32),//擦除flash
    EBUS_MCUBOOT_ERR_ENUM (*rom_write)(uint32,uint32,uint8 *),//写偏移地址，写长度，写buff指针
    EBUS_MCUBOOT_ERR_ENUM (*rom_read)(uint32,uint32,uint8 *),//偏移地址，读取长度，读buff指针
    void (*delay_ms)(uint32),//延时函数
    void (*mcu_reset)()//复位函数
){
    EBUS_MCUBOOT_ERR_ENUM err;
    uint32 add_offset,add_len;
		static uint32 data_len;
		//下列任意参数未定义则返回
    if(eBus_pack->datalen<1||write==0||rom_erasure==0 ||rom_write==0 ||rom_read==0)return ;
   
		switch(eBus_pack->data[0])
    {
        case EBUS_MCUBOOT_ENABLE: //0x70  进入BOOT模式
            eBus_mcuboot_state_enum=EBUS_MCUBOOT_STATE_ENUM_ENABLE;
            eBus_pack->data[0]=EBUS_MCUBOOT_ENABLE_ACK;
            EBUS_pack_2_flow(write,eBus_pack);
            break;
        
        case EBUS_MCUBOOT_REQUEST:   //0x74 请求升级
					if(eBus_pack->datalen!=5)  //此包数据区长度固定为5个字节
            {//数据长度校验
                eBus_pack->datalen=1;
                eBus_pack->data[0]=EBUS_MCUBOOT_DATA_PACKERR_ACK;
                EBUS_pack_2_flow(write,eBus_pack);
                break;
            }
            //计算升级包总大小，低位在前，高位在后
            add_len=eBus_pack->data[1]|(eBus_pack->data[2]<<8)|(eBus_pack->data[3]<<16)|(eBus_pack->data[4]<<24);
						data_len = add_len;
						//擦除升级标志，flash擦除是写把所有位写1;
						fmc_unlock();
						fmc_page_erase(UPDATE_FLAG_ADD);
						fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
						fmc_lock();
						
            //擦除flash操作
						err=rom_erasure(add_len);
            //响应
            eBus_pack->datalen=2;
            eBus_pack->data[0]=EBUS_MCUBOOT_REQUEST_ACK;
            eBus_pack->data[1]=err;//擦除状态
            EBUS_pack_2_flow(write,eBus_pack);
            break;
            
        case EBUS_MCUBOOT_RESET:   //boot重启命令
            if(eBus_pack->datalen!=1||mcu_reset==0)
            {
                eBus_pack->datalen=1;
                eBus_pack->data[0]=EBUS_MCUBOOT_DATA_PACKERR_ACK;
                EBUS_pack_2_flow(write,eBus_pack);
                break;
            }
            //响应
            eBus_pack->datalen=1;
            eBus_pack->data[0]=EBUS_MCUBOOT_RESET_ACK;
            EBUS_pack_2_flow(write,eBus_pack);
            delay_ms(20);
            mcu_reset();
            break;
                   
        case EBUS_MCUBOOT_DATA_WRITE:
            add_offset= eBus_pack->data[1]|(eBus_pack->data[2]<<8)|(eBus_pack->data[3]<<16)|(eBus_pack->data[4]<<24);
            add_len=    eBus_pack->data[5]|(eBus_pack->data[6]<<8)|(eBus_pack->data[7]<<16)|(eBus_pack->data[8]<<24);
            //判断写入数据长度和包长度是否吻合
            if(eBus_pack->datalen<=9||eBus_pack->datalen!=add_len+9)
            {
                eBus_pack->datalen=1;
                eBus_pack->data[0]=EBUS_MCUBOOT_DATA_PACKERR_ACK;
                EBUS_pack_2_flow(write,eBus_pack);
                break;
            }
            //程序写操作
            err=rom_write(add_offset,add_len,&(eBus_pack->data[9]));
						//当前包是否是最后一个包，如果是则升级成功
						if(data_len - add_offset <=512)
							{
								fmc_unlock();
								fmc_word_program(UPDATE_FLAG_ADD, 0);
								fmc_flag_clear((fmc_flag_enum)(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR));
								fmc_lock();
							}
            //响应
            eBus_pack->datalen=10;
            eBus_pack->data[0]=EBUS_MCUBOOT_DATA_WRITE_ACK;
            eBus_pack->data[9]=err;//写入状态
            EBUS_pack_2_flow(write,eBus_pack);
            //
            break;
        
        case EBUS_MCUBOOT_DATA_READ:
            if(eBus_pack->datalen!=9)
            {
                eBus_pack->datalen=1;
                eBus_pack->data[0]=EBUS_MCUBOOT_DATA_PACKERR_ACK;
                EBUS_pack_2_flow(write,eBus_pack);
                break;
            }
            add_offset= eBus_pack->data[1]|(eBus_pack->data[2]<<8)|(eBus_pack->data[3]<<16)|(eBus_pack->data[4]<<24);
            add_len=    eBus_pack->data[5]|(eBus_pack->data[6]<<8)|(eBus_pack->data[7]<<16)|(eBus_pack->data[8]<<24);
            err=rom_read(add_offset,add_len,eBus_pack->data+10);
            //
            eBus_pack->data[0]=EBUS_MCUBOOT_DATA_READ_ACK;
            eBus_pack->data[9]=err;//写入状态
            eBus_pack->datalen=10+add_len;
            EBUS_pack_2_flow(write,eBus_pack);
            break;

        default:break;
    }
}

//获取是否接收到boot处理指令
EBUS_MCUBOOT_STATE_ENUM eBus_mcuboot_state_get(void)
{
    return eBus_mcuboot_state_enum;
}

//获取升级成功标志	0：成功	1：不成功
uint32 eBus_flash_param_update_flag_get(void)
{
	return *(eBus_flash_param_struct.update_flag);
}




