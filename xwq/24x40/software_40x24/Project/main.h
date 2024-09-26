#ifndef __MAIN_H
#define __MAIN_H
#include "gd32e10x.h"
#include "74hc4051bq.h"
/* led spark function */

void led_spark(void);
void usbfs_send_buffer(void *cdc_acm, uint8_t *buf, int len_usb);

#pragma pack (1)
#define LOG_PRINTF 0

typedef struct {
    uint16_t sof;
    uint8_t tran_type;
    uint16_t len;
    uint8_t type;
    uint8_t adc_value[AX_NUM * AY_NUM];
    uint16_t checksum;
} SerialFrame_t;



#pragma pack ()


#endif /* __MAIN_H */
