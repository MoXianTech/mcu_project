#include "main.h"
#include "string.h"
#include "ws2812.h"
#include "systick.h"

// 定义PWM占空比对应的时序值
#define TIMING_ONE  90     // 代表二进制1的PWM占空比
#define TIMING_ZERO 45      // 代表二进制0的PWM占空比
#define RESET_LEN 100 // 约50us的0，具体根据PWM频率换算
#define num 10 // 假设只设置一个RGB灯
#define LED_DATA_LEN   (num * 24)
#define TOTAL_DATA_LEN (LED_DATA_LEN + RESET_LEN)
// DMA缓冲区，用于存储PWM占空比数据
uint16_t RGB_buffer[TOTAL_DATA_LEN];  
uint16_t RGB_buffer2[TOTAL_DATA_LEN];  //写入缓冲区
int dma0_ch0_idle = 0;

void DMA0_Channel0_IRQHandler(void);

// 配置定时器0通道0输出PWM
void timer11_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    
		rcu_periph_clock_enable(RCU_TIMER4);
	
		
	
    
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_MAX,GPIO_PIN_2);//GPIO KEY 初始化
//	gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP, ENABLE);

		timer_deinit(TIMER4);      //复位外设TIMERx
    /* TIMER0配置 */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 119;  // PWM周期
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER4, &timer_initpara);
    
    /* CH2配置为PWM0模式 */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_HIGH;
    timer_channel_output_config(TIMER4, TIMER_CH_2, &timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER4, TIMER_CH_2, 0);
    timer_channel_output_mode_config(TIMER4, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER4, TIMER_CH_2, TIMER_OC_SHADOW_ENABLE);
    
    /* 使能TIMER0主输出 */
    timer_primary_output_config(TIMER4, ENABLE);
    /* 使能TIMER0 CH0D DMA请求 */
    timer_dma_enable(TIMER4, TIMER_DMA_UPD);
    /* 禁用自动重载预装载 */
    timer_auto_reload_shadow_disable(TIMER4);
    /* 使能TIMER0计数器 */
    timer_enable(TIMER4);

}

// DMA配置
void dma11_config(void)
{
    dma_parameter_struct dma_init_struct;
    
    /* 使能DMA时钟 */
    rcu_periph_clock_enable(RCU_DMA1);
    
    /* 初始化DMA通道1 */
    dma_deinit(DMA1,DMA_CH1);
    
    /* DMA通道1初始化 */
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)RGB_buffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.number       = TOTAL_DATA_LEN; // 正确设置为300
    dma_init_struct.periph_addr  = (uint32_t)&TIMER_CH2CV(TIMER4);
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA1,DMA_CH1,&dma_init_struct);
    
    /* 配置DMA模式 */
    dma_circulation_disable(DMA1,DMA_CH1);
    dma_memory_to_memory_disable(DMA1,DMA_CH1);
		nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0); //RX
    dma_interrupt_enable(DMA1, DMA_CH1, DMA_INT_FTF);
		
    /* 使能DMA通道1 */
    dma_channel_disable(DMA1,DMA_CH1);
}
int test0 =0;
int test1 =0;
void DMA1_Channel1_IRQHandler(void)
{		
		test0++;
    if(dma_interrupt_flag_get(DMA1, DMA_CH1, DMA_INTF_FTFIF))
    {
        dma_interrupt_flag_clear(DMA1, DMA_CH1, DMA_INT_FLAG_FTF);
        dma0_ch0_idle = 0;
				test1++;
				dma_channel_disable(DMA1,DMA_CH1);
				timer_disable(TIMER4);
				timer_channel_output_shadow_config(TIMER4,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);
    }
}

// 设置RGB颜色值
void setRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t i = 0, j = 0;
    uint32_t rgb_value = green<<16 | red<<8 | blue;
    
//    while(dma0_ch0_idle == 1);
//		dma0_ch0_idle = 1;
//    dma_flag_clear(DMA1,DMA_CH1, DMA_INTF_FTFIF);
//    dma_channel_disable(DMA1,DMA_CH1);
//    dma_transfer_number_config(DMA1, DMA_CH1, sizeof(RGB_buffer)/sizeof(uint16_t));

    
    for (uint8_t j = 0; j < num; ++j) {
        for (i = 0; i < 24; ++i) {
            if ((rgb_value << i) & 0x800000) {
                RGB_buffer2[j * 24 + i] = TIMING_ONE;
            } else {
                RGB_buffer2[j * 24 + i] = TIMING_ZERO;
            }
        }
    }
		WS2812_Send();
//		for (int i = LED_DATA_LEN; i < TOTAL_DATA_LEN; ++i) {
//			RGB_buffer[i] = 0;
//		}


//    dma_channel_enable(DMA1,DMA_CH1);
}
void ws2812_process(void){
	dma11_config();
	timer11_config();
	
	setRGB(0,0,0);
}
void setSingleLEDColor(int index, uint8_t red, uint8_t green, uint8_t blue)
{
		while(dma0_ch0_idle ==1){
			return;
		};  // 等待DMA完成
    if ((index >= num)||(index <= -1)) return;  // 防止数组越界

    uint32_t rgb_value = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;

    for (uint8_t i = 0; i < 24; i++) {
        if ((rgb_value << i) & 0x800000) {
            RGB_buffer2[index * 24 + i] = TIMING_ONE;
        } else {
            RGB_buffer2[index * 24 + i] = TIMING_ZERO;
        }
    }
//		for (int i = LED_DATA_LEN; i < TOTAL_DATA_LEN; ++i) {
//			RGB_buffer[i] = 0;
//		}
		
}
void setOneLEDColor(int index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index >= num) return;  // 防止数组越界

		// 先把所有LED关掉（设为0）
    for (int i = 0; i < num * 24; i++) {
        RGB_buffer2[i] = TIMING_ZERO;
    }
    // 设置指定灯的颜色
    uint32_t rgb_value = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
    for (uint8_t i = 0; i < 24; i++) {
        if ((rgb_value << i) & 0x800000) {
            RGB_buffer2[index * 24 + i] = TIMING_ONE;
        } else {
            RGB_buffer2[index * 24 + i] = TIMING_ZERO;
        }
    }
}

void WS2812_Send(void)
{
		if (memcmp(RGB_buffer, RGB_buffer2, sizeof(RGB_buffer)) != 0) {
			memcpy(RGB_buffer, RGB_buffer2, sizeof(RGB_buffer));
		}
		else 
			return;
		
    while(dma0_ch0_idle == 1);
		dma0_ch0_idle = 1;
//		memcpy(RGB_buffer, RGB_buffer2, sizeof(RGB_buffer));
//		memset(RGB_buffer2, 0, sizeof(RGB_buffer2));  // 确保缓冲区初始为0
    dma_flag_clear(DMA1,DMA_CH1, DMA_INTF_FTFIF);
    timer_enable(TIMER4);
		timer_channel_output_shadow_config(TIMER4,TIMER_CH_2,TIMER_OC_SHADOW_ENABLE);

    dma_transfer_number_config(DMA1, DMA_CH1, TOTAL_DATA_LEN);
		dma_channel_enable(DMA1,DMA_CH1);
}

void ledAnimation(void) {
    // 1. 依次点亮绿灯
    for(uint8_t i = 0; i < num; i++) {
        setSingleLEDColor(i, 0, 255, 0);  // 绿色
        WS2812_Send();
        delay_1ms(50);  // 每个LED点亮间隔50ms
    }
    
    // 短暂保持全绿
    delay_1ms(500);
    
    // 2. 闪两次红灯
    for(uint8_t flash = 0; flash < 2; flash++) {
        setRGB(255, 0, 0);  // 全红
        delay_1ms(250);
        setRGB(0, 0, 0);    // 全灭
        delay_1ms(250);
    }
}

//void
#define MEDIAN_WINDOW 5  // 建议使用奇数

int median_filter(int channel, int new_sample) {
    static int samples[5][MEDIAN_WINDOW] = {0};
    static int index[5] = {0};
    int temp[MEDIAN_WINDOW];
    
    // 存储新样本
    samples[channel][index[channel]] = new_sample;
    index[channel] = (index[channel] + 1) % MEDIAN_WINDOW;
    
    // 复制到临时数组排序
    for(int i=0; i<MEDIAN_WINDOW; i++) {
        temp[i] = samples[channel][i];
    }
    
    // 简单排序
    for(int i=0; i<MEDIAN_WINDOW-1; i++) {
        for(int j=i+1; j<MEDIAN_WINDOW; j++) {
            if(temp[i] > temp[j]) {
                int swap = temp[i];
                temp[i] = temp[j];
                temp[j] = swap;
            }
        }
    }
    
    return temp[MEDIAN_WINDOW/2];  // 返回中值
}