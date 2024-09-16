#ifndef _AIRSAC_H
#define _AIRSAC_H
#include "lin.h"
#define LIN0_PID		0X03

typedef enum
{
		POWER_ON,
		UP_CHARGE,
		DOWN_CHARGE,
		ALL_CHARGE,
		ALL_DISCHARGE,
		//FLANK_CHARGE,
		//FLANK_DISCHARGE,
		MASSAGE,
//		PATTERN1,
//		PATTERN2,
//		PATTERN3,
//		PATTERN4,
//		PATTERN5,
//		PATTERN6,
//		PATTERN7,
//		PATTERN8,
//		PATTERN9,
//		PATTERN10,
//		STRENGTH1,
//		STRENGTH2,
//		STRENGTH3,
		AIR_MODE_MAX_NUM
}air_mode_enum;

void airsac_thread_init(void);
void set_airsac_mode(air_mode_enum mode);

void airsac_thread_run(void);
#endif
