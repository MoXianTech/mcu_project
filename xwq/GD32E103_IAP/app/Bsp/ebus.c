#include "ebus.h"
#include "uart.h"


EBUS_STATE_STRUCT ebus_state_struct;
EBUS_STRUCT eBus_pack;

//�������� EBUS_ERR_NONE ��ʾ�յ�һ���������ݰ�
EBUS_ERR_ENUM  EBUS_flow_2_struct(//������ת��Ϊһ������֡
EBUS_STATE_STRUCT *sate,    //����֡ת��״̬���ṹ��ָ�룩
EBUS_STRUCT *pack,          //����ָ֡�� ���ṹ��ָ�룩
uint16 divtime_ms               //��������ʱ���������룩
)
{
			uint8 data;
			while(USART1_Recv(&data, 1) != 0)
			{
					sate->time = 0;
					switch(sate->step)
					{
							case 0:
									if(data==EBUS_HEAD1)sate->step=10;//�ж�֡ͷ
									break;
							case 10:
									if(data==EBUS_HEAD2)sate->step=20;//�ж�֡ͷ
									else if(data!=EBUS_HEAD1)sate->step=0;
									break;
							case 20: //��ȡ������
									pack->ptype=data;//��ȡ֡����
									pack->sumnot=data;//��ʼ��У��ֵ
									sate->sit=0;        //��ʼ������λ��
									sate->step=30;
									break;
							case 30://��ȡ��������1
									pack->datalen=(data&0x7F);
							//��ǰ���������ֽ�bit7λΪ��ʱ����ʾ����һ���ֽ�Ҳ�Ǳ�ʾ�������ȡ�
									if((data&0x80)==0x80) 
									{
											sate->step=32;
									}
									else 
									{
											if(pack->datalen>EBUS_MAX_PACK_LENGTH)sate->step=0;//���ݳ��ȳ����������
											else sate->step=40;
									}
									pack->sumnot+=data;
									break;
							case 32: //��ȡ��������2
									if((data&0x80)==0x80)
									{
											if(data==EBUS_HEAD1)sate->step=10;
											else sate->step=0;
									}
									else 
									{
											pack->datalen|=(data<<7);
											if(pack->datalen>EBUS_MAX_PACK_LENGTH) sate->step=0;//���ݳ��ȳ����������
											else if (pack->datalen == 0) sate->step = 50;
											else sate->step=40;
											
									}
									pack->sumnot+=data;
									break;
							case 40: //���������data���ݣ�ֱ���������
									pack->data[sate->sit] = data;//��ȡ֡����
									pack->sumnot += data;
									sate->sit++;
									if(sate->sit>=pack->datalen)  //���ݽ������
									{
											sate->step=50;
									}
									break;
									
							case 50:
									pack->sumnot=~pack->sumnot;
									sate->step=0;
									if(pack->sumnot==data)//�ж�У��
									{
											return EBUS_ERR_ENUM_NONE;  //У��ɹ�������
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
					sate->time+=divtime_ms;//Э�����ʱ���ۼ�
					if(sate->time>sate->timeout)//��������ʱ
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
			case EBUS_MCUBOOT_ENABLE: //0x70  ����BOOTģʽ
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
    //���ð�������ʱʱ�� ����
    ebus_state_struct.timeout=100;
}


//���ٺ������һ��
void bootloader_check(uint16 time_ms)
{
		EBUS_ERR_ENUM state;
		//��ȡ���ڽ��յ����ݲ��ж�
		state = EBUS_flow_2_struct(&ebus_state_struct,&eBus_pack,time_ms);
		if(state == EBUS_ERR_ENUM_NONE)  //EBUS_ERR_ENUM_NONE��ʾ���ݽ��������У��ͨ��
		{
			switch(eBus_pack.ptype)
			{
					case EBUS_PACK_TYPE_APP_UPGRADE:		//0xF0	���� APP������
						eBus_mcuboot_run(&eBus_pack);
						break;
			}
		}
}
