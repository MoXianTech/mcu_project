#ifndef __PID_MOTO_CONTROL_H__
#define __PID_MOTO_CONTROL_H__

#include "bsp_moto.h"
#include "PID_base.h"

void set_moto_thread_status(uint8_t enable);
void PID_thread_init(void);
void moto_PID_thread_run(void);
void set_moto_target(MOTO_NUM moto_num, int16_t pos_value);
void set_moto_stop(MOTO_NUM moto_num);

#endif
