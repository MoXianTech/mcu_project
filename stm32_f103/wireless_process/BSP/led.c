//=============================================================================
//文件名称:led.h
//文件概要:LED初始化
//库版本：V3.5.0
//版权所有:源地工作室 http://www.vcc-gnd.com/  网店 http://vcc-gnd.taobao.com/
//版本更新:2013-10-09 V1.0
//=============================================================================


//头文件
#include "led.h"


//=============================================================================
//函数名称: LED_GPIO_Config(void)
//功能概要:LED灯引脚配置
//参数名称:无
//函数返回:无
//=============================================================================
void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

