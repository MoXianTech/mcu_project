/**
* @file                hal_tk.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.6.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_TK_H
#define HAL_TK_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "csa37f7x_tk.h"
#include "hal_config.h"
/**********************************************/
typedef struct
{
    uint32_t hal_tk_rawdata[TK_CH_NUM];
    uint8_t  hal_tk_data_valid[TK_CH_NUM];
}tk_data_config_t;
#define BIT(X)  ((uint32_t)(1)<<(X))


extern uint16_t tk_g_afe_channel[] ;
extern uint8_t tk_g_ainn_channel[] ;
extern uint8_t tk_g_ainp_channel[] ;
extern int16_t hal_tk_afe_rawdata[];

void hal_tk_init(void);
void hal_tk_sample(void);
int16_t hal_tk_sample_get_rawdata(uint8_t ch);
void hal_tk_enable(void);
void hal_tk_disable(void);
void hal_tk_calibration_all(void);
uint32_t hal_tk_calibration_ch(uint8_t ChannelIdx);
int tk_dac_calibration(uint8_t cvcnum);
void hal_tk_interrput(void) ;
int16_t hal_tk_sample_get_adc_accumulate(uint8_t ch);
float afe2_tk_second_calibration_cdac_get(int16_t adc_value, uint16_t cpin);
void fw_auto_calibration_proc_tk(void);
#endif /* HAL_TK_H */

/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
