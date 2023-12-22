//UTF-8
/*



*/
#ifndef _EBUS_CORE_H_
#define _EBUS_CORE_H_


/*自定义简单协议*/
/*
版本:V1.0
日期
*/

#include "eBus_port.h"

//数据包格式
/*
描述：     head    head    Ptype       datalen     data[]      sunnot
字节数：    1       1       1             1           x           1
            FE      EF      XX          
sunnot=~(type+cmd+datalen+data...)
*/

#define EBUS_SYS_MEMORT_LENGTH                      32

//版本
#define EBUS_VES                                    1
//头1
#define EBUS_HEAD1                                  0xFE
//头2
#define EBUS_HEAD2                                  0xEF
//数据长度
#define EBUS_MAX_PACK_LENGTH                        EBUS_PORT_MAX_PACK_LENGTH

//从流获取一个字节数据，返回0表示获取失败
typedef uint8(*EBUS_FLOW_READ_PFUNC)(uint8 *value);
//写一个字节到流
typedef uint8(*EBUS_FLOW_WRITE_PFUNC)(uint8 value);

typedef enum
{
    //无错误
    EBUS_ERR_ENUM_NONE=0,
    //超时
    EBUS_ERR_ENUM_OVERTIME,
    //超长
    EBUS_ERR_ENUM_OVERLEN,
    //等待
    EBUS_ERR_ENUM_WAIT,
    //数据源出错
    EBUS_ERR_ENUM_DATA_SRC,
}EBUS_ERR_ENUM;



typedef union{
    uint16 eBus_ves;
    uint16 pack_max_len;
    uint8 res[28];
}EBUS_MEMORY_STRUCT;


typedef struct
{
    //包类型
    uint8 ptype;
    //参数长度
    uint16 datalen;
    //参数实体
    uint8 data[EBUS_MAX_PACK_LENGTH];
    //累加和校验和取反
    uint8 sumnot;
}EBUS_STRUCT;


typedef struct
{    
    uint8 step;         ////当前解析步骤   
    uint16 time;        ////解析函数调用时间周期，毫秒    
    uint16 timeout;     ////包解析，超时时间，毫秒   
    uint16 sit;        //参数写入数据临时位置
	
}EBUS_STATE_STRUCT;     //协议解析状态结构体


typedef struct
{
		uint32 *update_flag;			//升级成功标志
}EBUS_FLASH_PARAM_STRUCT;		//掉电保存参数结构体



#define EBUS_PACK_TYPE_READ_MEMORY                  0x02
#define EBUS_PACK_TYPE_WRITE_MEMORY_DACK            0x03
#define EBUS_PACK_TYPE_WRITE_MEMORY_EACK            0x04


#define EBUS_PACK_TYPE_DEVICE_CONTROL               0x20


#define EBUS_PACK_TYPE_READ_MEMORY_ACK              0xE0
#define EBUS_PACK_TYPE_WRITE_MEMORY_ACK             0xE1

#define EBUS_PACK_TYPE_APP_UPGRADE                  0xF0











//函数返回EBUS_ERR_NONE表示收到一个完整数据包
EBUS_ERR_ENUM  EBUS_flow_2_struct(//数据流转换为一个数据帧
EBUS_FLOW_READ_PFUNC flowread,    //数据流读一个字节
EBUS_STATE_STRUCT *sate,    //数据帧转换状态
EBUS_STRUCT *pack,          //数据帧指针
uint16 divtime_ms               //函数调用时间间隔（毫秒）
);


uint16  EBUS_creat_flow(//通过已知数据创建数据流
EBUS_FLOW_WRITE_PFUNC write,//数据流写函数
uint8 ptype,//帧类型
uint8 *data,//帧数据
uint16 datalen//帧数据长度
);

uint16  EBUS_pack_2_flow(//通过已知数据包创建数据流
EBUS_FLOW_WRITE_PFUNC write,//数据流写函数
EBUS_STRUCT *pack
);


uint16  EBUS_creat_flow_d0(EBUS_FLOW_WRITE_PFUNC write,uint8 type);
uint16  EBUS_creat_flow_d1(EBUS_FLOW_WRITE_PFUNC write,uint8 type,uint8 data);
uint16  EBUS_creat_flow_d2(EBUS_FLOW_WRITE_PFUNC write,uint8 type,uint8 data1,uint8 data2);
uint16  EBUS_creat_flow_uint16_LSB(EBUS_FLOW_WRITE_PFUNC write,uint8 type,uint16 data);

#endif
