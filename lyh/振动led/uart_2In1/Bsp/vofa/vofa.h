#ifndef __VOFA_h
#define __VOFA_h
#include "main.h"
typedef struct {
    float run_strength;          // 振动强度
    float run_time;          // 运行时间
    uint8_t press;          		// 按下
} MyStruct;

MyStruct get_vofa_date(void);
#endif
