#include "74HC4051BQ.h"
#include "adc.h"
#include "math.h"
#include "string.h"
#include <stdio.h>

#define ADC_DELAY_TIME (7 * 220) //9 ns * 120
// 定义GPIO引脚
#define A_PIN GPIO_PIN_13
#define B_PIN GPIO_PIN_14
#define C_PIN GPIO_PIN_15
#define ENABLE_PIN GPIO_Pin_6

#define GPIO_PORT GPIOB

uint8_t mx_presure[4];

void Divider_Resistance_Selection(uint8_t data);
void Hc4051Delay(uint32_t num)
{
    while(num --);
}

/*
功能：Init
输入：none
*/ 
void Hc4051IoInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
}

void Set_Channel(uint8_t channel) {
    // 使能设备
//    GPIO_ResetBits(GPIO_PORT, ENABLE_PIN);

    // 根据通道设置A、B、C引脚
    switch (channel) {
        case 0: // X0
            gpio_bit_reset(GPIO_PORT, A_PIN | B_PIN | C_PIN);
            break;
        case 1: // X1
            gpio_bit_set(GPIO_PORT, A_PIN);
            gpio_bit_reset(GPIO_PORT, B_PIN | C_PIN);
            break;
        case 2: // X5
            gpio_bit_set(GPIO_PORT, C_PIN | A_PIN);
            gpio_bit_reset(GPIO_PORT, B_PIN);
            break;
        case 3: // X7
            gpio_bit_set(GPIO_PORT, A_PIN | B_PIN | C_PIN);
            break;
//        default:
//            // 禁用设备
//            GPIO_SetBits(GPIO_PORT, ENABLE_PIN);
//            break;
    }
}
void HC_timer6_config(void)
{
    // 1. 使能定时器时钟
    rcu_periph_clock_enable(RCU_TIMER6);

    // 2. 初始化定时器参数
    timer_parameter_struct timer_initpara;
    timer_struct_para_init(&timer_initpara);
    
    timer_initpara.prescaler         = 120 - 1;          // 预分频系数
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 49;           // 自动重装载值
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER6, &timer_initpara);

    // 3. 使能更新中断
    timer_interrupt_enable(TIMER6, TIMER_INT_UP);
    timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);

    // 4. 配置NVIC
    nvic_irq_enable(TIMER6_IRQn, 1, 0);

    // 5. 启动定时器
    timer_enable(TIMER6);
		Set_Channel(0);
}

void get_mx_data(){
	static int channel = 0;
	mx_presure[channel] = return_adc_value();
	channel++;
	if(channel>3) channel = 0;
	Set_Channel(channel);
	
}
void output_mx_data(uint8_t *get_out){
//	printf("samples:%d, %d, %d, %d\n", mx_presure[0], mx_presure[1], mx_presure[2], mx_presure[3]);
	memcpy(get_out, mx_presure, sizeof(mx_presure));
	
}
void TIMER6_IRQHandler(void)
{
    if(timer_interrupt_flag_get(TIMER6, TIMER_INT_FLAG_UP) != RESET)
    {
        timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);
        
			
				get_mx_data();
			
    }
}
