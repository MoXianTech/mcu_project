/*
 *-----------------------------------------------------------------------------
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from  CHIPSEA.
 *
 *            (C) COPYRIGHT 2021 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.
 *                ALL RIGHTS RESERVED
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from CHIPSEA.
 *
 *      Release Information :forcetouch fw driver head file
 *      version : v0.1
 *-----------------------------------------------------------------------------
 */

#ifndef CS_PRESS_DRIVER_H
#define CS_PRESS_DRIVER_H


#define AFE_USE_CH      10
#define KEY_NUM         1

// struct
typedef struct{
    short rawdata[AFE_USE_CH];
	short baselinedata[AFE_USE_CH];
	short forcedata[AFE_USE_CH];
}CS_RAWDATA_Def;
typedef struct{
    unsigned char device_id[10];
    unsigned short manufacturer_id;
    unsigned short module_id;
    unsigned short fw_version;
}cs_fw_info_def_t;


typedef struct{
    unsigned short sync;
    unsigned short seg1;
    unsigned short seg2;
    unsigned short seg3;
    unsigned short version;
    unsigned short seg4;
    unsigned short seg5;
    unsigned short length;
}CS_FW_HEAD_Def;

typedef struct{
    unsigned short sync;
    unsigned short version;
    unsigned short seg1;
    unsigned short length;
}cs_fw_head_def_m_t;

// functions
char cs_press_reset_ic(void);
char cs_press_iic_rw_test(unsigned char test_data);
char cs_press_read_fw_info(cs_fw_info_def_t *fw_info);
char cs_press_fw_force_update(const unsigned char *fw_array);
char cs_press_fw_force_async_update(const unsigned char *fw_array,const unsigned short len);
char cs_press_fw_high_version_update(const unsigned char *fw_array);
char cs_read_rawdata_bytes(unsigned char *rawdata, unsigned char *rawLen);
void cs_press_set_devie_address(unsigned char dev_address);
char cs_press_read_rawdata_init(void);
char cs_press_read_rawdata(CS_RAWDATA_Def *rawdata);

// fw data
static const unsigned char cs_default_fw_array[]={
/*user program*/

};



/* endian swap for 16bit */
#define BIG_LITTLE_SWAP16(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
                             (((uint16_t)(A) & 0x00ff) << 8))
/* endian swap for 32bit */
#define BIG_LITTLE_SWAP32(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                             (((uint32_t)(A) & 0x00ff0000) >> 8) | \
                            (((uint32_t)(A) & 0x0000ff00) << 8) | \
                            (((uint32_t)(A) & 0x000000ff) << 24))

#define CS_PRESS_DEV_ADDR_M                 0xE4
#define CS_PRESS_DEV_ADDR_F                 0xA0

#define RSTPIN_RESET_ENABLE             0
#define SOFT_RESET_ENABLE               0

#define RETRY_NUM                       5
#define DEBUG_MODE_DELAY_TIME           20

#define CS_CHIP_ID_LENGTH               10
#define CS_MANUFACTURER_ID_LENGTH       2
#define CS_MODULE_ID_LENGTH             2
#define CS_FW_VERSION_LENGTH            2

#define AFE_MAX_CH                      20

#define DEBUG_MODE_REG                  0x60
#define DEBUG_READY_REG                 0x61
#define DEBUG_DATA_REG                  0x62

#define FW_ADDR_CODE_LENGTH             0x0e
#define FW_ADDR_VERSION                 0x08
#define FW_ADDR_CODE_START              0x100
#define FW_ADDR_CODE_START_M            32

#define FW_ONE_BLOCK_LENGTH_W           128
#define FW_ONE_BLOCK_LENGTH_R           256
#define FW_ONE_BLOCK_LENGTH_R_M         32

#define AP_RESET_MCU_REG                0x01
#define AP_DEVICE_ID_REG                0x02
#define AP_MANUFACTURER_ID_REG          0x03
#define AP_MODULE_ID_REG                0x04
#define AP_VERSION_REG                  0x05
#define AP_WAKEUP_REG                   0x06
#define AP_SLEEP_REG                    0x07

#define AP_CALIBRATION_REG              0x1c
#define AP_WATCH_MODE_REG               0x1d
#define AP_RW_TEST_REG                  0x1f
#define AP_FORCEDATA_REG                0x20

#define AP_W_CAL_FACTOR_DEBUG_MODE      0x30
#define AP_R_CAL_FACTOR_DEBUG_MODE      0x31

#define AP_W_PRESS_LEVEL_DEBUG_MODE     0x32
#define AP_R_PRESS_LEVEL_DEBUG_MODE     0x33

#define AP_CALIBRATION_DEBUG_MODE       0x34
#define AP_R_SENSOR_STATUS_DEBUG_MODE   0x36
#define AP_R_OFFSET_DEBUG_MODE          0x37

#define AP_R_RAWDATA_DEBUG_MODE         0x01

#define AP_R_NOISE_DEBUG_MODE           0x11
#define AP_R_PROCESSED_DEBUG_MODE       0x14

#define BOOT_CMD_REG                    0x0000
#define BOOT_CMD_REG_M                  0x21
#define BOOT_RESET_REG                  0xF17C
#define BOOT_CMD_LENGTH                 4
#define BOOT_CMD_LENGTH_M               3

#define CALIBRATION_SUCCESS_FALG        0xf0
#define CALIBRATION_FAIL_FALG           0xf2
#define CALIBRATION_OVERTIME_FALG       0xff

#define AP_R_DEBUGDATA_DEBUG_MODE       0xf4
#define AP_R_RAWDATA_MODE         		0xF4
#define AP_R_AFE_DEBUG_MODE             0x04


#endif

