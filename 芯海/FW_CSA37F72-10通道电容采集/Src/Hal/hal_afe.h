/**
* @file                hal_afe.c
* @brief
* @author
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_AFE_H
#define HAL_AFE_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

typedef enum
{
    PGA_1 = 0x01,
    PGA_2 = 0x00,
} PGA_x_TypeDef;

typedef enum
{
    AFE_0 = 0x00,
    AFE_1 = 0x01,
} AFE_x_TypeDef;

typedef struct
{
    uint8_t PGA1_Gain;
    uint8_t PGA2_Gain;
    uint16_t ratio;           //ratio  = (PGA1_Gain * PGA2_Gain) / 6.912
} PGAGain_st;

extern uint16_t g_afe_channel[];
extern uint8_t g_ainn_channel[];
extern uint8_t g_ainp_channel[];
extern int16_t hal_afe_rawdata[];
extern uint16_t DACOffset[];

void hal_afe_init(void);
uint8_t hal_afe_info_get(uint8_t *info_data);
void hal_afe_interrput(void);
uint8_t hal_afe_ch_num_get(void);
int16_t hal_afe_get_rawdata(int ch);
void hal_afe_sample(void);
void hal_afe_sample_single(uint8_t ch);
uint16_t hal_get_sensor_state( uint8_t ChannelIdx);
uint8_t hal_afe_get_adc_overflow(void);
void hal_afe_calibration_ch(uint16_t *afe_channel_t, uint8_t ChannelIdx);

uint16_t hal_afe_temperature_sample(uint8_t ChannelIdx);
int16_t hal_get_inadc_value(void);
#endif /*__HAL_AFE_H */


/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
