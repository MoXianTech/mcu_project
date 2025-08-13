/**
* @file                framework_debug.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup Framework层
  * @{
  */
/**
 * @defgroup debug模块
 * @brief debug模块用于输出调试信息
 * @{
 */

//hal
#include "hal_afe.h"
#include "hal_uart.h"
#include "hal_config.h"
#include "hal_temperature.h"
//framework
#include "framework_debug.h"
#include "framework_notify.h"
#include "framework_reg.h"
#include "framework_sample.h"
//config
#include "user_config.h"

#include "framework_sample_tk.h"

#if DEBUG_PRINT_EN
#define fw_printf   printf
#else
#define fw_printf
#endif

static uint8_t flag_debug_output_enable = DEBUG_PRINT_DEFAULT_OUTPUT;
static uint8_t flag_debug_iic_output_enable = DEBUG_PRINT_IIC_DEFAULT_OUTPUT;

static int8_t *fw_debug_iic_printf_read_buffer;
static int8_t *fw_debug_iic_printf_buffer;
static int8_t fw_debug_iic_printf_buffer_bank0[FW_DEBUG_IIC_BUFFER_NUM];        // 双bank缓存，避免漏数据
static int8_t fw_debug_iic_printf_buffer_bank1[FW_DEBUG_IIC_BUFFER_NUM];
static int8_t fw_debug_iic_bank_count = 0;

char fw_debug_iic_printf_receive[FW_DEBUG_IIC_BUFFER_NUM];
/* 先留这里,重新实现iic_printf后干掉,直接用recive数组即可 */
#define DEBUG_IIC_PRINT_REVICE_DATA                 fw_debug_iic_printf_receive // ap可使用，每次从头写入要打印的数据
#define s_printf_buf fw_debug_iic_printf_receive
static uint16_t fw_debug_iic_printf_point = 0;

/**
* @fn void fw_debug_init(void)
* @brief debug system init
* @param None
* @return None.
*/
void fw_debug_init(void)
{
    fw_debug_iic_printf_buffer = fw_debug_iic_printf_buffer_bank0;
    fw_printf("UART debug info start!\r\n");

    fw_printf_iic(sprintf(DEBUG_IIC_PRINT_REVICE_DATA, "IIC debug info start!\r\n"));
}

/**
* @fn void fw_printf_array_int16(int16_t *array, uint8_t num)
* @brief print array data
* @param [in]array: point to array data, [in]num: array data num
* @return None.
*/
void fw_printf_array_int16(int16_t *array, uint16_t num)
{
#if DEBUG_PRINT_EN
    uint16_t i;
    for(i = 0; i < num; i++)
    {
        fw_printf("%d ", array[i]);
    }
#endif
}

/**
* @fn void fw_printf_array_hex32(int32_t *array, uint16_t num)
* @brief print array data
* @param [in]array: point to array data, [in]num: array data num
* @return None.
*/
void fw_printf_array_hex32(uint32_t *array, uint16_t num)
{
#if DEBUG_PRINT_EN
    uint16_t i;
    for(i = 0; i < num; i++)
    {
        fw_printf("%8x ", array[i]);
    }
#endif
}

/**
* @fn void fw_printf_enable(uint8_t enable)
* @brief enable uart output
* @param None
* @return None.
*/
void fw_printf_out_enable(void)
{
    flag_debug_output_enable = 1;
}

/**
* @fn void fw_printf_out_disable(void)
* @brief disable uart output
* @param None
* @return None.
*/
void fw_printf_out_disable(void)
{
    flag_debug_output_enable = 0;
}

/**
* @fn void fw_printf_enable(uint8_t enable)
* @brief enable uart output
* @param None
* @return None.
*/
void fw_printf_iic(uint16_t num)
{
    uint16_t i;

    if(flag_debug_iic_output_enable)
    {
        for(i = 0; i < num; i++)
        {
            if(fw_debug_iic_printf_point >= FW_DEBUG_IIC_BUFFER_NUM)
            {
                fw_debug_iic_printf_point = 0;
            }
            fw_debug_iic_printf_buffer[fw_debug_iic_printf_point++] = fw_debug_iic_printf_receive[i];
        }
#if FW_DEBUG_IIC_NOTIFY
        fw_notify_int_out_once();           // 输出中断通知
#endif
    }
}

/**
* @fn uint16_t fw_printf_iic_get_data_num(void)
* @brief enable uart output
* @param None
* @return None.
*/
uint16_t fw_printf_iic_get_data_num(void)       // 获取iic printf 有效 数据个数
{
    uint16_t temp;

    if(fw_debug_iic_bank_count)                 // 读取有效个数说明即将读取数据，切换缓存bank
    {
        fw_debug_iic_printf_buffer = fw_debug_iic_printf_buffer_bank0;
        fw_debug_iic_printf_read_buffer = fw_debug_iic_printf_buffer_bank1;     // 锁定读数据

        fw_debug_iic_bank_count = 0;
    }
    else
    {
        fw_debug_iic_printf_buffer = fw_debug_iic_printf_buffer_bank1;
        fw_debug_iic_printf_read_buffer = fw_debug_iic_printf_buffer_bank0;     // 锁定读数据

        fw_debug_iic_bank_count = 1;
    }

    temp = fw_debug_iic_printf_point;

    fw_debug_iic_printf_point = 0;              // 读取后清零，切换bank重新计数

    return temp;
}

/**
* @fn void fw_printf_enable(uint8_t enable)
* @brief enable uart output
* @param None
* @return None.
*/
uint8_t fw_printf_iic_get_data(uint16_t num)
{
    return fw_debug_iic_printf_read_buffer[num];
}


/**
* @fn void fw_printf_enable(uint8_t enable)
* @brief enable uart output
* @param None
* @return None.
*/
void fw_printf_iic_out_enable(void)
{
    flag_debug_iic_output_enable = 1;
}

/**
* @fn void fw_printf_out_disable(void)
* @brief disable uart output
* @param None
* @return None.
*/
void fw_printf_iic_out_disable(void)
{
    flag_debug_iic_output_enable = 0;
}

/**
* @fn void fw_sample_print(void)
* @brief iic print rawdata
* @param None
* @return None.
*/


extern int16_t calibrate_offset[];
extern uint32_t calibrate_cnt;
extern int16_t calibrate_trg_cnt[];

void fw_sample_data_print(void)
{
    //    uint8_t i;
    static uint8_t first_flag = 0;
    static int16_t base_buff[8];
    static uint8_t data_buff[39];
    // debug
    int offset = 0;
	
//	offset += sprintf(s_printf_buf + offset, "%5d,", hal_tk_sample_get_rawdata(0));
//	offset += sprintf(s_printf_buf + offset, "%5d,", hal_tk_sample_get_rawdata(1));
//	offset += sprintf(s_printf_buf + offset, "%5d,", hal_tk_sample_get_rawdata(2));
//	offset += sprintf(s_printf_buf + offset, "%5d,", hal_tk_sample_get_rawdata(3));
//	offset += sprintf(s_printf_buf + offset, "%5d,", hal_tk_sample_get_rawdata(4));
//	offset += sprintf(s_printf_buf + offset, "%5d,", hal_tk_sample_get_rawdata(5));
//	offset += sprintf(s_printf_buf + offset, "%5d,", hal_tk_sample_get_rawdata(6));
	offset += sprintf(s_printf_buf + offset, "%5d,", hal_afe_get_rawdata(0));
	offset += sprintf(s_printf_buf + offset, "%5d,", hal_afe_get_rawdata(1));

	offset = strlen(s_printf_buf);
	strcpy(s_printf_buf + offset, "\r\n");
	fw_printf("%s", s_printf_buf);
	
}


//-------------------------------------------------------------------------

#pragma import(__use_no_semihosting)

struct __FILE
{
    int a;
};

FILE __stdout;
void _sys_exit(int x)
{

}

int fputc(int ch, FILE *stream)
{
    if(flag_debug_output_enable)
    {
        hal_uart_send_byte((uint8_t)ch);
    }

    return ch;
}

/**
  * @}
  */

/**
  * @}
  */
