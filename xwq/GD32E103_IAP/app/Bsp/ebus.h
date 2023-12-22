#ifndef 	__EBUS_H__
#define	__EBUS_H__

#ifndef uint64
#define uint64 unsigned long long
#endif

#ifndef int64
#define int64 signed long long
#endif

#ifndef uint32
#define uint32 unsigned int 
#endif

#ifndef int32
#define int32 signed int 
#endif


#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef int16
#define int16 short
#endif

#ifndef int8
#define int8 char
#endif

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
#define EBUS_MAX_PACK_LENGTH                        1100

#define EBUS_PACK_TYPE_APP_UPGRADE	0XF0

//进入BOOT模式
#define EBUS_MCUBOOT_ENABLE                 0x70


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

void eBus_init(void);
void bootloader_check(uint16 time_ms);


#endif
