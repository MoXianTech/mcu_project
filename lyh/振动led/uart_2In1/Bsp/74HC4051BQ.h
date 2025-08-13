#ifndef __74HC4051B_h
#define __74HC4051B_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"
void Hc4051IoInit(void);
void HC_timer6_config(void);
void output_mx_data(uint8_t *get_out);
#endif


