
#ifndef __SN74LV4051_H
#define __SN74LV4051_H	 

#include	"config.h"

sbit SN74_INH = P1^2;
sbit SN74_A = P1^3;
sbit SN74_B = P1^4;
sbit SN74_C = P1^5;

enum type_sn74l
{
    Y0 = 0x00,
    Y1 = 0x01,
    Y2 = 0x02,
    Y3 = 0x03,
    Y4 = 0x04,
	Y5 = 0x05,
    Y6 = 0x06,
	Y7 = 0x07
};

void Sn74lv4051Driver(u8 index_output);

#endif