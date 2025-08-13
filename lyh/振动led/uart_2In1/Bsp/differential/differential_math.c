#include "differential_math.h"
#include "uart.h"
int differential_init(Differential_t *differential, 
	uint16_t sampling_interval_ms, 
	uint16_t min_duration_ms,  
	uint16_t max_duration_ms, 
	int16_t rise_threshold, 
	int16_t saturation_threshold, 
	int16_t fall_threshold,
	uint16_t debounce_time_ms )
{
	differential->current_input=0;        				
  differential->previous_input=0; 		
  differential->delta  = 0;
	differential->event_duration_ms=0;
	differential->elapsed_time_ms = 0;	
	differential->next_valid_time_ms = 0;	
	differential->quiet_time = 0;
	differential->sampling_interval_ms = sampling_interval_ms;
	differential->min_duration_ms = min_duration_ms;
	differential->max_duration_ms = max_duration_ms;
	differential->rise_threshold = rise_threshold;
	differential->saturation_threshold = saturation_threshold;
	differential->fall_threshold = fall_threshold;
	differential->get_time = 0;
	differential->state = 0;
	differential->debounce_time_ms  = debounce_time_ms ;
	return 1;
}


void differential_run(Differential_t *differential,uint16_t current_input){
	differential->current_input = current_input;
	differential->delta  = differential->current_input - differential->previous_input;
	differential->previous_input = differential->current_input;
	differential->elapsed_time_ms += differential->sampling_interval_ms;
	if(differential->elapsed_time_ms < differential->next_valid_time_ms){
		return;
	}
	if(differential->delta  >= differential->saturation_threshold){
				differential->state = 0;
				differential->event_duration_ms = 0;
				differential->quiet_time = 0;
				differential->next_valid_time_ms = differential->elapsed_time_ms + differential->debounce_time_ms ;
				return;
		}
	
	switch(differential->state){//观测阈值，切换状态
		case 0: 
			if(differential->delta  >= differential->rise_threshold){
				differential->state = 1;
			}
			break;
		case 1:
			if(differential->delta  <= differential->fall_threshold){
				differential->state = 2;
			}
			break;
	}
	if(differential->state == 1 || differential->state == 2){
		
		differential->event_duration_ms += differential->sampling_interval_ms;
	}
	if(differential->state == 2){
		if(differential->event_duration_ms >= differential->min_duration_ms && differential->event_duration_ms <= differential->max_duration_ms){
			
			differential->get_time++;
			differential->state = 0;
			differential->event_duration_ms = 0;
			differential->event_duration_ms = 0;
			differential->quiet_time = 0;
			differential->next_valid_time_ms = differential->elapsed_time_ms + differential->debounce_time_ms ;
		}
		else{
			differential->state = 0;
			differential->event_duration_ms = 0;
			differential->event_duration_ms = 0;
			differential->quiet_time = 0;
			differential->next_valid_time_ms = differential->elapsed_time_ms + differential->debounce_time_ms ;
		}
	}
	
	
}
#define AVG_WINDOW 10
int avg_filter(int channel, int new_sample) {
    static int buf[5][AVG_WINDOW] = {0};
    static int index[5] = {0};
    static int sum[5] = {0};

    // 更新 sum
    sum[channel] -= buf[channel][index[channel]];
    buf[channel][index[channel]] = new_sample;
    sum[channel] += new_sample;

    // 滚动索引
    index[channel] = (index[channel] + 1) % AVG_WINDOW;

    return sum[channel] / AVG_WINDOW;
}
int check_button(Differential_t *differential){//检测被按下的次数
	
	static int times = 0;
	if((times != differential->state) &&(differential->state != 2)){
		times = differential->state;
		
		return 1;
	}
	return 0;
}