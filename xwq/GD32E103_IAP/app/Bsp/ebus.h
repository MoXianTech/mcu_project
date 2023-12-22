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

//���ݰ���ʽ
/*
������     head    head    Ptype       datalen     data[]      sunnot
�ֽ�����    1       1       1             1           x           1
            FE      EF      XX          
sunnot=~(type+cmd+datalen+data...)
*/

#define EBUS_SYS_MEMORT_LENGTH                      32

//�汾
#define EBUS_VES                                    1
//ͷ1
#define EBUS_HEAD1                                  0xFE
//ͷ2
#define EBUS_HEAD2                                  0xEF
//���ݳ���
#define EBUS_MAX_PACK_LENGTH                        1100

#define EBUS_PACK_TYPE_APP_UPGRADE	0XF0

//����BOOTģʽ
#define EBUS_MCUBOOT_ENABLE                 0x70


typedef enum
{
    //�޴���
    EBUS_ERR_ENUM_NONE=0,
    //��ʱ
    EBUS_ERR_ENUM_OVERTIME,
    //����
    EBUS_ERR_ENUM_OVERLEN,
    //�ȴ�
    EBUS_ERR_ENUM_WAIT,
    //����Դ����
    EBUS_ERR_ENUM_DATA_SRC,
}EBUS_ERR_ENUM;

typedef struct
{
    //������
    uint8 ptype;
    //��������
    uint16 datalen;
    //����ʵ��
    uint8 data[EBUS_MAX_PACK_LENGTH];
    //�ۼӺ�У���ȡ��
    uint8 sumnot;
}EBUS_STRUCT;


typedef struct
{    
    uint8 step;         ////��ǰ��������   
    uint16 time;        ////������������ʱ�����ڣ�����    
    uint16 timeout;     ////����������ʱʱ�䣬����   
    uint16 sit;        //����д��������ʱλ��
	
}EBUS_STATE_STRUCT;     //Э�����״̬�ṹ��

void eBus_init(void);
void bootloader_check(uint16 time_ms);


#endif
