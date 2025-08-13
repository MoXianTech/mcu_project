/**
* @file                hal_timer.c
* @brief
* @author
* @version             V0.1.0
* @date                2018.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

void hal_timer_delay_init(void);
void hal_timer_delay_ms(int time_ms);
void hal_timer_delay_us(int time_us);
void hal_timer_init(unsigned short int timer);
void hal_timer_counter_set(unsigned short int timer);
void hal_timer_config_hclk_36m(void);
#endif /*__HAL_TIMER_H_ */







