/*************************************************************************
	> File Name: 74hc595.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/9/20 17:48:31
 ************************************************************************/

#include "stdint.h"

#define IO_MAX_NUM	30//所扩展的io数目

void ic_74hc595_delay(uint32_t num);
void ic_74hc595_init(void);
void ic_74hc595_thread(void);
void ic_74hc595_clean(void);
void ic_74hc595_io_write(uint8_t io_num);
