/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

#ifndef		__TYPE_DEF_H
#define		__TYPE_DEF_H

//========================================================================
//                               ���Ͷ���
//========================================================================

typedef unsigned char   u8;     //  8 bits 
typedef unsigned int    u16;    // 16 bits 
typedef unsigned long   u32;    // 32 bits 

typedef signed char     int8;   //  8 bits 
typedef signed int      int16;  // 16 bits 
typedef signed long     int32;  // 32 bits 

typedef unsigned char   uint8;  //  8 bits 
typedef unsigned int    uint16; // 16 bits 
typedef unsigned long   uint32; // 32 bits 

typedef unsigned char		uint8_t;
typedef unsigned int		uint16_t;
typedef unsigned long		uint32_t;
//===================================================

#define	TRUE	1
#define	FALSE	0

//===================================================

#define	NULL	0

//===================================================

#define	Priority_0			0	//�ж����ȼ�Ϊ 0 ������ͼ���
#define	Priority_1			1	//�ж����ȼ�Ϊ 1 �����ϵͼ���
#define	Priority_2			2	//�ж����ȼ�Ϊ 2 �����ϸ߼���
#define	Priority_3			3	//�ж����ȼ�Ϊ 3 ������߼���

#define ENABLE		1
#define DISABLE		0

#define SUCCESS		0
#define FAIL		-1

//===================================================

#define	I2C_Mode_Master			1
#define	I2C_Mode_Slave			0

#define	PIE			0x20	//1: �ȽϽ����0��1, �����������ж�
#define	NIE			0x10	//1: �ȽϽ����1��0, �����½����ж�

#define	PWMA	128
#define	PWMB	129

//===================================================

#endif