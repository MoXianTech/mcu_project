#include "eBus_core.h"


EBUS_MEMORY_STRUCT eBus_memory_struct={
.eBus_ves=EBUS_VES,
.pack_max_len=EBUS_MAX_PACK_LENGTH
};

uint8 sys_memory[EBUS_SYS_MEMORT_LENGTH]={
1
};

//函数返回 EBUS_ERR_NONE 表示收到一个完整数据包
EBUS_ERR_ENUM  EBUS_flow_2_struct(//数据流转换为一个数据帧
EBUS_FLOW_READ_PFUNC flowread,    //数据流读一个字节（回调函数调用读取）
EBUS_STATE_STRUCT *sate,    //数据帧转换状态（结构体指针）
EBUS_STRUCT *pack,          //数据帧指针 （结构体指针）
uint16 divtime_ms               //函数调用时间间隔（毫秒）
)
{
    uint8 data;
    if(flowread==0)  return EBUS_ERR_ENUM_DATA_SRC;
    while(flowread(&(data))==0)  //回调函数 simplefifo_buff_pt_read 返回0  //0：SIMPLEFIFO_ERROR_NONE
    {
        sate->time=0;  //重置解析时间，防止超时
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


uint16  EBUS_creat_flow(//通过已知数据创建数据流
EBUS_FLOW_WRITE_PFUNC write,//数据流写函数
uint8 ptype,//帧类型
uint8 *data,//帧数据
uint16 datalen//帧数据长度
)
{   uint16 plen=0;
    uint8 check=0;
    uint16 i;
    if(datalen>EBUS_MAX_PACK_LENGTH||write==0)return 0;
    //
    write(EBUS_HEAD1);
    write(EBUS_HEAD2);
    write(ptype);check+=ptype;
    if(datalen>127)
    {
        //计算长度低7位
        plen=datalen|0x80;
        write(plen);check+=plen;
        //计算长度高7位
        plen=(datalen>>7)&0x7F;
        write(plen);check+=plen;
        plen=datalen+6;
    }
    else 
    {
        plen=datalen&0x7F;
        write(plen);check+=plen;
        plen=datalen+5;
    }
    for(i=0;i<datalen;i++)
    {
        write(data[i]);check+=data[i];
    }
    check=~check;
    write(check);
    return plen;
}

uint16  EBUS_pack_2_flow(//通过已知数据包创建数据流
EBUS_FLOW_WRITE_PFUNC write,//数据流串口发送函数
EBUS_STRUCT *pack
)
{   uint16 plen=0;
    uint16 i;
    if(pack->datalen>EBUS_MAX_PACK_LENGTH||write==0)return 0;
    //
    write(EBUS_HEAD1);
    write(EBUS_HEAD2);
    
    write(pack->ptype);pack->sumnot=pack->ptype;
    if(pack->datalen>127)
    {
        //计算长度低7位
        plen=pack->datalen|0x80;
        write(plen);pack->sumnot+=plen;
        //计算长度高7位
        plen=(pack->datalen>>7)&0x7F;
        write(plen);pack->sumnot+=plen;
        plen=pack->datalen+6;
    }
    else 
    {
        plen=pack->datalen&0x7F;
        write(plen);pack->sumnot+=plen;
        plen=pack->datalen+5;
    }
    for(i=0;i<pack->datalen;i++)
    {
        write(pack->data[i]);pack->sumnot+=pack->data[i];
    }
    pack->sumnot=~pack->sumnot;
    write(pack->sumnot);
    return plen;
}


uint16  EBUS_creat_flow_d0(EBUS_FLOW_WRITE_PFUNC write,uint8 type)
{
    return EBUS_creat_flow(write,type,0,0);
}
//
uint16  EBUS_creat_flow_d1(EBUS_FLOW_WRITE_PFUNC write,uint8 type,uint8 data)
{
    return EBUS_creat_flow(write,type,&data,1);
}

uint16  EBUS_creat_flow_d2(EBUS_FLOW_WRITE_PFUNC write,uint8 type,uint8 data1,uint8 data2)
{
    uint8 data[2]={data1,data2};
    return EBUS_creat_flow(write,type,data,2);
}
uint16  EBUS_creat_flow_uint16_LSB(EBUS_FLOW_WRITE_PFUNC write,uint8 type,uint16 data)
{
    uint8 datax[2]={data,data>>8};
    return EBUS_creat_flow(write,type,datax,2);
}

