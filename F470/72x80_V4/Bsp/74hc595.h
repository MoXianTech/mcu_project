/*************************************************************************
	> File Name: 74hc595.h
	> Author: ma6174
	> Mail: ma6174@163.com
	> Created Time: 2023/9/20 17:48:31
 ************************************************************************/

#include "stdint.h"
#include "resi_math_lib.h"
#include "stdbool.h"



void ic_74hc595_move_bit(int8_t bit);
void ic_74hc595_delay(uint32_t num);
void ic_74hc595_init(SCAN_LEVEL_T scan_level);
void ic_74hc595_thread(void);
void ic_74hc595_move_clean(bool output_enable);
void ic_74hc595_set_io_scan(int8_t io_num);
SCAN_LEVEL_T get_scan_mode(void);
