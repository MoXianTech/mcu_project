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

#include	"STC8G_H_Timer.h"
#include	"STC8H_PWM.h"

/*************	���ر�������	**************/
bit PWM1_Flag;
bit PWM2_Flag;
bit PWM3_Flag;
bit PWM4_Flag;

/*************  �ⲿ�����ͱ������� *****************/
extern PWMx_Duty PWMA_Duty;

//========================================================================
// ����: Timer0_ISR_Handler
// ����: Timer0�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-23
//========================================================================
void Timer0_ISR_Handler (void) interrupt TMR0_VECTOR		//���ж�ʱ�Ѿ������־
{
	// TODO: �ڴ˴������û�����
	if(!PWM1_Flag)
	{
		PWMA_Duty.PWM1_Duty++;
		if(PWMA_Duty.PWM1_Duty >= 2047) PWM1_Flag = 1;
	}
	else
	{
		PWMA_Duty.PWM1_Duty--;
		if(PWMA_Duty.PWM1_Duty <= 0) PWM1_Flag = 0;
	}

	if(!PWM2_Flag)
	{
		PWMA_Duty.PWM2_Duty++;
		if(PWMA_Duty.PWM2_Duty >= 2047) PWM2_Flag = 1;
	}
	else
	{
		PWMA_Duty.PWM2_Duty--;
		if(PWMA_Duty.PWM2_Duty <= 0) PWM2_Flag = 0;
	}

	if(!PWM3_Flag)
	{
		PWMA_Duty.PWM3_Duty++;
		if(PWMA_Duty.PWM3_Duty >= 2047) PWM3_Flag = 1;
	}
	else
	{
		PWMA_Duty.PWM3_Duty--;
		if(PWMA_Duty.PWM3_Duty <= 0) PWM3_Flag = 0;
	}

	if(!PWM4_Flag)
	{
		PWMA_Duty.PWM4_Duty++;
		if(PWMA_Duty.PWM4_Duty >= 2047) PWM4_Flag = 1;
	}
	else
	{
		PWMA_Duty.PWM4_Duty--;
		if(PWMA_Duty.PWM4_Duty <= 0) PWM4_Flag = 0;
	}
	
	UpdatePwm(PWMA, &PWMA_Duty);
}

//========================================================================
// ����: Timer1_ISR_Handler
// ����: Timer1�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-23
//========================================================================
void Timer1_ISR_Handler (void) interrupt TMR1_VECTOR		//���ж�ʱ�Ѿ������־
{
	// TODO: �ڴ˴������û�����
	P66 = ~P66;
}

//========================================================================
// ����: Timer2_ISR_Handler
// ����: Timer2�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-23
//========================================================================
void Timer2_ISR_Handler (void) interrupt TMR2_VECTOR		//���ж�ʱ�Ѿ������־
{
	// TODO: �ڴ˴������û�����
	P65 = ~P65;
}

//========================================================================
// ����: Timer3_ISR_Handler
// ����: Timer3�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-23
//========================================================================
void Timer3_ISR_Handler (void) interrupt TMR3_VECTOR		//���ж�ʱ�Ѿ������־
{
	// TODO: �ڴ˴������û�����
	P64 = ~P64;
}

//========================================================================
// ����: Timer4_ISR_Handler
// ����: Timer4�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-23
//========================================================================
void Timer4_ISR_Handler (void) interrupt TMR4_VECTOR		//���ж�ʱ�Ѿ������־
{
	// TODO: �ڴ˴������û�����
	P63 = ~P63;
}