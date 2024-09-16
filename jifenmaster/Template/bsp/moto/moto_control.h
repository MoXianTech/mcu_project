#ifndef _MOTO_CONTROL_H
#define _MOTO_CONTROL_H

#include "bsp_encoder.h"
#include "bsp_moto.h"
#include "gd32f4xx.h"
#include "board.h"


typedef struct
{
	uint8_t	direction;
	uint32_t speed;
	uint32_t location;
}moto_para_stu;


#endif
