#ifndef __DIFFERENTIAL_MATH_h
#define __DIFFERENTIAL_MATH_h
#include <stdint.h>
#include "string.h"
#include "stdlib.h"
typedef struct {
    uint8_t current_input;        							//输入变量（自变量）
    uint8_t previous_input; 								//上次输入
		int16_t delta ; 					//微分值
		uint32_t event_duration_ms;								//上升沿抓取时间
		uint32_t elapsed_time_ms;							//运行时间
		uint32_t next_valid_time_ms;					//滤波执行时间
		uint32_t quiet_time;								//静默时间
		uint16_t sampling_interval_ms;						//执行周期
		uint16_t debounce_time_ms ;							//滤波周期
		uint16_t min_duration_ms; 						//最小时间阈值
		uint16_t max_duration_ms; 						//最大时间阈值
		int16_t rise_threshold;		//微分正向阈值
		int16_t saturation_threshold;			//微分最大阈值，
		int16_t fall_threshold;		//微分负向阈值
		uint16_t get_time;									//达成次数
		uint8_t state;											//当达到正向阈值后切换为1，当达到负向阈值切换到2，完成数据更迭后切换为原始态0
}Differential_t;

int differential_init(
	Differential_t *differential, 
	uint16_t sampling_interval_ms, 
	uint16_t min_duration_ms,  
	uint16_t max_duration_ms, 
	int16_t rise_threshold, 
	int16_t saturation_threshold, 
	int16_t fall_threshold,
	uint16_t debounce_time_ms 
);
void differential_run(Differential_t *differential,uint16_t current_input);
int avg_filter(int channel, int new_sample);
int check_button(Differential_t *differential);
#endif

