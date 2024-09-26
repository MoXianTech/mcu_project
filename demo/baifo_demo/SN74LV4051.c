
#include "SN74LV4051.h"

/*index_output: 0~7*/
void Sn74lv4051Driver(u8 index_output)
{
	SN74_INH = 0;//低电平使能
	SN74_A = index_output & 0x01;
	SN74_B = (index_output & 0x02) >> 1;
	SN74_C = (index_output & 0x04) >> 2;
}

