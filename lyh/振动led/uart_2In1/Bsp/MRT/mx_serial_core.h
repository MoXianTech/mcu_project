#ifndef __MX_SERIAL_CORE_H
#define __MX_SERIAL_CORE_H
#include "uart.h"
//#include "bsp_uart_ringbuffer.h"
#include "string.h"

#define SERIAL_HEAD	0x5AA5
#define FRAME_HEAD	0XA55A

#define MX_SERIAL_BUFF_SIZE		2048

typedef enum 
{
	DATA_SRC_ERR,
	SOF_ERR,
	DATA_LEN_OVER,
	FRAME_LEN_LITTLE,
	CHECK_SUM_ERR,
	REV_SUCCESSFUL,	
}mx_frame_rev_err;

typedef enum
{
	SOF_GET = 0,
	TRANTYPE_GET,
	DATALEN_GET,
	TYPE_GET,
	DATA_GET,
	CHECKSUM_GET,
}mx_frame_rev_enum;

typedef struct
{
	uint8_t trantype;
  uint16_t datalen;
	uint8_t type;
	uint8_t *data;
}mx_serial_struct;


typedef struct
{
	uint16_t sof;
	uint8_t trantype;
  uint16_t datalen;
	uint8_t type;
	uint8_t *data;
	uint16_t checksum;
}mx_frame_rx_struct;

typedef struct
{
	uint8_t input_trantype;
	uint16_t input_datalen;
	uint8_t input_type;
	uint8_t *input_data;
	uint8_t *out_frame;
	uint16_t out_fram_len;
}mx_frame_tx_struct;

//uint16_t  mx_serial_rev_run(uart_type_def com, mx_serial_struct * mx_serial);
//uint8_t mx_send_frame(uart_type_def com, mx_serial_struct * mx_serial);
int get_pressure_all(void);
uint8_t mx_frame_rev(uint8_t* rev_buff, uint16_t rev_buff_len, mx_frame_rx_struct *mx_frame);
//uint8_t mx_serial_rev_run(uart_type_def com, mx_frame_rx_struct * mx_serial_rx);
//uint8_t mx_send_frame(uart_type_def com, mx_frame_tx_struct * mx_serial_tx);
#endif

