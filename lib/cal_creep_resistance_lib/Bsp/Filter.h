#ifndef _FILTER_H
#define _FILTER_H

#include 	"main.h"
#include 	"string.h"
#include 	"stdlib.h"

#define MEAN_COUNT				1				//ȡ1,3,4,6,10
#define MEAN_BUFFER_LEN		POS_X*POS_Y



void get_filter_value(uint8_t *source_buffer,uint8_t *target_buffer,uint16_t buffer_len);


#endif
