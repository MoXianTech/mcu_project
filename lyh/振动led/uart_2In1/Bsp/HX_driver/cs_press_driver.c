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
 *      Release Information : forcetouch fw driver source file
 *      version : v0.1
 *-----------------------------------------------------------------------------
 */

#include "cs_press_driver.h"
#include "systick.h"
#include "soft_i2c.h"

/* F series iic addr used for default */
unsigned char gDeviceAddress = CS_PRESS_DEV_ADDR_F;

/* used for F series */
unsigned char boot_fw_write_cmd[BOOT_CMD_LENGTH] = {0xaa,0x55,0xa5,0x5a};
unsigned char boot_fw_reset_cmd[BOOT_CMD_LENGTH] = {0xa0,0x5f,0x01,0x00};
unsigned char boot_fw_wflag_cmd[BOOT_CMD_LENGTH] = {0x50,0x41,0x53,0x73};

/* used for M series */
unsigned char boot_fw_write_cmd_m[] = {0x55,0xa5,0x5a};
unsigned char boot_fw_read_cmd_m[] = {0x55,0xa5,0xa5};
unsigned char boot_fw_jump_cmd_m[] = {0x55,0xa5,0xaa};
unsigned char boot_fw_reset_cmd_m[] = {0x55,0xa5,0x55};
unsigned char boot_fw_wflag_cmd_m[] = {0x55,0xa5,0x51};

/* used for all */
unsigned char boot_status_reg_dat[4] ={0x10,0x01,0x18,0x60};
unsigned char boot_status_reg_dat_vaildbit[4] = {0xf0,0xff,0xff,0xf0};

unsigned char ap_status_reg_dat[4] ={0x80,0x81,0x18,0x60};
unsigned char ap_status_reg_dat_vaildbit[4] = {0xff,0xff,0xff,0xf0};

/**
  * @brief  set force touch iic address
  * @param  iic addr
  * @retval None
  */
void cs_press_set_devie_address(unsigned char dev_address)
{
    gDeviceAddress = dev_address;
}

/**
  * @brief  delay function
  * @param  time_ms: delay time, unit:ms
  * @retval None
  */
static void cs_press_delay_ms(unsigned int time_ms)
{
		
    // user program 
		delay_1ms(time_ms);
}

/**
  * @brief  ic power on function
  * @param  None
  * @retval None
  */
static void cs_press_power_up(void)
{
    // user program 
}

/**
  * @brief  ic power down function
  * @param  None
  * @retval None
  */
static void cs_press_power_down(void)
{
    // user program 
}

/**
  * @brief  ic rst pin set high
  * @param  None
  * @retval None
  */
static void cs_press_rstpin_high(void)
{
	
	
    // user program 
    
}

/**
  * @brief  ic rst pin set low
  * @param  None
  * @retval None
  */
static void cs_press_rstpin_low(void)
{
    // user program 
  
}

#ifdef SOFT_IIC
#include "soft_i2c.h"
/**
  * @brief  iic write funciton
  * @param  regAddress: reg data, *dat: point to data to write, length: write data length
  * @retval 0:success, -1: fail
  */
char cs_press_iic_write(unsigned char regAddress , unsigned char *dat, unsigned int length)
{
    unsigned char ret;
    
    // user program 

    ret = ISendStr(gDeviceAddress,regAddress,dat,length);

    if(!ret)
    {
        ret = -1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}

/**
  * @brief  iic read funciton
  * @param  regAddress: reg data, *dat: read data buffer, length: read data length
  * @retval 0:success, -1: fail
  */
char cs_press_iic_read(unsigned char regAddress ,unsigned char *dat, unsigned int length)
{
    unsigned char ret;
    
    // user program 

    ret = IRcvStr(gDeviceAddress,regAddress,dat,length);

    if(!ret)
    {
        ret = -1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}
/**
  * @brief  iic write 16bit address funciton
  * @param  regAddress: reg data, *dat: point to data to write, length: write data length
  * @retval 0:success, -1: fail
  */
static char cs_press_iic_write_double_reg(unsigned short regAddress ,const unsigned char *dat, unsigned int length)
{
    unsigned char ret;
    
    // user program 
    ret = ISendStr_16Reg(gDeviceAddress,regAddress,(unsigned char *)dat,length);

    if(!ret)
    {
        ret = -1;
    }
    else
    {
        ret = 0;
    }
    return ret;
}

/**
  * @brief  iic read 16bit address funciton
  * @param  regAddress: reg data, *dat: read data buffer, length: read data length
  * @retval 0:success, -1: fail
  */
char cs_press_iic_read_double_reg(unsigned short regAddress ,unsigned char *dat, unsigned int length)
{
    unsigned char ret;
	
    // user program 
    ret = IRcvStr_16Reg(gDeviceAddress,regAddress,dat,length);

    if(!ret)
    {
        ret = -1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}
#else
/**
  * @brief  iic write funciton
  * @param  regAddress: reg data, *dat: point to data to write, length: write data length
  * @retval 0:success, -1: fail
  */
char cs_press_iic_write(unsigned char regAddress , const unsigned char *dat, unsigned int length)
{
 HXf72_write_bytes(regAddress,(uint8_t *)dat,length);
	return 0;
// user program
}

/**
  * @brief  iic read funciton
  * @param  regAddress: reg data, *dat: read data buffer, length: read data length
  * @retval 0:success, -1: fail
  */
char cs_press_iic_read(unsigned char regAddress ,const unsigned char *dat, unsigned int length)
{
  // user program
	char get_f72 = -1;
	HXf72_read_bytes(regAddress,(uint8_t *)dat,length);
	return 0;
}

/**
  * @brief  iic write 16bit address funciton
  * @param  regAddress: reg data, *dat: point to data to write, length: write data length
  * @retval 0:success, -1: fail
  */
static char cs_press_iic_write_double_reg(unsigned short regAddress ,const unsigned char *dat, unsigned int length)
{
// user program

}

/**
  * @brief  iic read 16bit address funciton
  * @param  regAddress: reg data, *dat: read data buffer, length: read data length
  * @retval 0:success, -1: fail
  */
static char cs_press_iic_read_double_reg(unsigned short regAddress ,unsigned char *dat, unsigned int length)
{
// user program

}
#endif

/**
  * @brief  wakeup iic
  * @param  None
  * @retval 0:success, -1: fail
  */
static char cs_press_wakeup_iic(void)
{
    char ret;

    ret = cs_press_iic_rw_test(0x67);

    return ret;
}

/**
  * @brief  clean debug mode reg, debug ready reg
  * @param  None
  * @retval 0:success, -1: fail
  */
static char cs_press_clean_debugmode(void)
{
    char ret = 0;
    unsigned char temp_data = 0;

    ret |= cs_press_iic_write(DEBUG_MODE_REG, &temp_data, 1);
    ret |= cs_press_iic_write(DEBUG_READY_REG, &temp_data, 1);

    return ret;
}

/**
  * @brief  set debug mode reg
  * @param  mode_num: debug mode num data
  * @retval 0:success, -1: fail
  */
static char cs_press_set_debugmode(unsigned char mode_num)
{
    char ret;

    /* clear DEBUG_READY_REG before writing DEBUG_MODE_REG */
    unsigned char tmp = 0;
    ret = cs_press_iic_write(DEBUG_READY_REG, &tmp, 1);
    if(ret)
    {
        return -1;
    }

    ret = cs_press_iic_write(DEBUG_MODE_REG, &mode_num, 1);
    return ret;
}

/**
  * @brief  set debug ready reg
  * @param  ready_num: debug ready num data
  * @retval 0:success, -1: fail
  */
static char cs_press_set_debugready(unsigned char ready_num)
{
    char ret;

    ret = cs_press_iic_write(DEBUG_READY_REG, &ready_num, 1);
    
    return ret;
}

/**
  * @brief  get debug ready reg data
  * @param  None
  * @retval ready reg data
  */
static unsigned char cs_press_get_debugready(void)
{
    char ret;
    unsigned char ready_num = 0;

    ret = cs_press_iic_read(DEBUG_READY_REG, &ready_num, 1);

    if(ret != 0)
    {
        ready_num = 0;
    }

    return ready_num;
}

/**
  * @brief  write debug data
  * @param  *debugdata: point to data buffer, length: write data length
  * @retval 0:success, -1: fail
  */
static char cs_press_write_debugdata(unsigned char *debugdata, unsigned char length)
{
    char ret;

    ret = cs_press_iic_write(DEBUG_DATA_REG, debugdata, length);
    
    return ret;
}

/**
  * @brief  read debug data
  * @param  *debugdata: point to data buffer, length: write data length
  * @retval 0:success, -1: fail
  */
static char cs_press_read_debugdata(unsigned char *debugdata, unsigned char length)
{
    char ret;

    ret = cs_press_iic_read(DEBUG_DATA_REG, debugdata, length);

    return ret;
}

/**
  * @brief  soft_reset the device
  * @param  None
  * @retval 0:success, -1: fail
  */
static char cs_press_soft_reset_device(void)
{
    char ret = 0;
    unsigned char retry = RETRY_NUM;
    /* M series required write value 0xcc */
    unsigned char temp_data = 0xcc;

    /* repeatly reset including first time i2c wake up */
    do
    {
        if(ret!=0)
        {
            cs_press_delay_ms(1);
        }
        ret = cs_press_iic_write(AP_RESET_MCU_REG, &temp_data, 1);
    }while((ret != 0 ) && (retry--));

    return ret;
}

/**
  * @brief  reset ic
  * @param  None
  * @retval 0:success, -1: fail
  */
char cs_press_reset_ic(void)
{
    char ret = 0;
    cs_press_iic_rw_test(0x12); // wake up iic
    #if SOFT_RESET_ENABLE
        ret = cs_press_soft_reset_device();
        /* in case in boot mode F */
        if(gDeviceAddress == CS_PRESS_DEV_ADDR_F)
        {
            cs_press_iic_write_double_reg(BOOT_RESET_REG ,boot_fw_reset_cmd, BOOT_CMD_LENGTH);
        }
    #else
        #if RSTPIN_RESET_ENABLE
            cs_press_rstpin_high();
            cs_press_delay_ms(100);
            cs_press_rstpin_low();

        #else// hw reset ic
            cs_press_power_down();
            cs_press_delay_ms(100);
            cs_press_power_up();

        #endif

    #endif
    // cs_press_delay_ms(60);
    return ret;
}

/**
  * @brief  check ic status for m series
  * @param  None
  * @retval 0:boot, 1: ap fw, -1: error
  */
static char cs_press_check_fw_status_m(void)
{
	int i;
    #define FW_STATUS_REG                   0x00
    unsigned char ret;
    unsigned char read_data[4];
    unsigned char flag;
    /* rw_test return value not checked,in case already in boot mode */
    cs_press_iic_rw_test(0x12); // wake up iic
    cs_press_delay_ms(3);

    ret = cs_press_iic_read(FW_STATUS_REG, read_data, 4);
 
    if(ret == 0)
    {
        // check boot status
        flag = 0;
        for(i = 0; i < 4; i++)
        {
            if(boot_status_reg_dat[i] != (read_data[i] & boot_status_reg_dat_vaildbit[i]))
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            return 0;   // boot status
        }

        // check ap status
        flag = 0;
        for(i = 0; i < 4; i++)
        {
            if(ap_status_reg_dat[i] != (read_data[i] & ap_status_reg_dat_vaildbit[i]))
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            return 1;   // ap status
        }
    }

    return (char)-1;  // error
}

/**
  * @brief  write and read iic test reg
  * @param  test_data: test data
  * @retval 0:success, -1: fail
  */
char cs_press_iic_rw_test(unsigned char test_data)
{
    char ret = -1;
    char iic_ret = -1;
    unsigned char retry = RETRY_NUM;
    unsigned char read_data = 0;

    do
    {

        iic_ret = cs_press_iic_write(AP_RW_TEST_REG, &test_data, 1);
        if(iic_ret)
        {
            continue;
        }

        iic_ret = cs_press_iic_read(AP_RW_TEST_REG, &read_data, 1);
        if(iic_ret)
        {
            continue;
        }

        //如果该行log没有出现证明前面的iic读写有错误

        if(read_data != test_data)
        {
            ret = -1;
        }
        else
        {
            ret = 0;
        }

    }while((ret != 0) && (retry--));

    return ret;
}


/**
  * @brief  read fw info
  * @param  *fw_info: point to info struct
  * @retval 0:success, -1: fail
  */
char cs_press_read_fw_info(cs_fw_info_def_t *fw_info)
{
    char ret = 0;
    cs_press_wakeup_iic();

    // read fw info
    ret = cs_press_iic_read(AP_DEVICE_ID_REG, (unsigned char *)&fw_info->device_id, CS_CHIP_ID_LENGTH);
    if(ret != 0)
    {
        return -1;
    }
    // Manufacturer id
    ret = cs_press_iic_read(AP_MANUFACTURER_ID_REG, (unsigned char *)&fw_info->manufacturer_id, CS_MANUFACTURER_ID_LENGTH);
    if(ret != 0)
    {
        return -1;
    }
    // Module id
    ret = cs_press_iic_read(AP_MODULE_ID_REG, (unsigned char *)&fw_info->module_id, CS_MODULE_ID_LENGTH);
    if(ret != 0)
    {
        return -1;
    }
    // FW Version
    ret = cs_press_iic_read(AP_VERSION_REG, (unsigned char *)&fw_info->fw_version, CS_FW_VERSION_LENGTH);
    if(ret !=0 )
    {
        return -1;
    }
    return 0;
}

/**
  * @brief  计算和校验
  * @param  sum_pre: 上次计算的校验值
  * @param  *data: 本次需要校验的数据指针
  * @param  len: 本次需要校验的数据长度
  * @retval void
  */
static unsigned char calc_check_sum(unsigned char sum_pre,const unsigned char *data,unsigned short len)
{
	int i;
    unsigned char checksum = sum_pre;
    for(i = 0; i < len; i++)
    {
        checksum += data[i];
    }
    return checksum;
}

/**
  * @brief  forced firmware update
  * @param  *fw_array: point to fw hex array
  * @retval 0:success, -1: fail
  */
char cs_press_fw_force_update(const unsigned char *fw_array)
{
    unsigned int i, j;
    char ret;
    unsigned int fw_code_length;
    unsigned int fw_block_num_w, fw_block_num_r;
    const unsigned char *fw_code_start;
    unsigned int fw_count;
    unsigned char fw_read_code[FW_ONE_BLOCK_LENGTH_R];
    unsigned short fw_default_version;
    unsigned short fw_read_version;

    // fw init
    fw_code_length = ((((unsigned short)fw_array[FW_ADDR_CODE_LENGTH+0]<<8)&0xff00)|fw_array[FW_ADDR_CODE_LENGTH+1]); 
    fw_code_start = &fw_array[FW_ADDR_CODE_START];
    fw_block_num_w = fw_code_length/FW_ONE_BLOCK_LENGTH_W;
    fw_block_num_r = fw_code_length/FW_ONE_BLOCK_LENGTH_R;
    fw_default_version = ((((unsigned short)fw_array[FW_ADDR_VERSION+0]<<8)&0xff00)|fw_array[FW_ADDR_VERSION+1]);

   

#if SOFT_RESET_ENABLE
    cs_press_reset_ic();
    cs_press_delay_ms(80);
#else
    #if RSTPIN_RESET_ENABLE         // rst pin resto
        cs_press_rstpin_high();
        cs_press_delay_ms(50);
        cs_press_rstpin_low();
        cs_press_delay_ms(80);
    #else                           // hw reset ic
        cs_press_power_down();
        cs_press_delay_ms(100);
        cs_press_power_up();
        cs_press_delay_ms(80);
    #endif
#endif
    //cs_press_soft_reset_device();
    //cs_press_reset_ic();
    //cs_press_delay_ms(20);

    // send fw write cmd
    cs_press_iic_write_double_reg(BOOT_CMD_REG ,boot_fw_write_cmd, BOOT_CMD_LENGTH);
    cs_press_delay_ms(2000);	// waiting flash erase

    // send fw code 
    fw_count = 0;

    for(i=0;i<fw_block_num_w;i++)
    {
        ret = cs_press_iic_write_double_reg(i*FW_ONE_BLOCK_LENGTH_W, fw_code_start+fw_count, FW_ONE_BLOCK_LENGTH_W);
        
        fw_count += FW_ONE_BLOCK_LENGTH_W;
        
        if(ret != 0)    
        {
            goto FLAG_FW_FAIL;
        }

        cs_press_delay_ms(20);
    }

    // read & check fw code
    fw_count = 0;

    for(i=0;i<fw_block_num_r;i++)
    {
        // read code data
        ret = cs_press_iic_read_double_reg(i*FW_ONE_BLOCK_LENGTH_R, fw_read_code, FW_ONE_BLOCK_LENGTH_R);

        if(ret != 0)
        {
            goto FLAG_FW_FAIL;
        }

        // check code data
        for(j=0;j<FW_ONE_BLOCK_LENGTH_R;j++)
        {
            if(fw_read_code[j] != fw_code_start[fw_count+j])
            {
                goto FLAG_FW_FAIL;
            }
        }
        fw_count += FW_ONE_BLOCK_LENGTH_R;
        cs_press_delay_ms(20);
    }

    // send fw flag cmd
    cs_press_iic_write_double_reg(BOOT_CMD_REG ,boot_fw_wflag_cmd, BOOT_CMD_LENGTH);
    cs_press_delay_ms(50); 
    cs_press_reset_ic();

    // check fw version
    cs_press_delay_ms(300); // skip boot
    
    ret = cs_press_iic_read(AP_VERSION_REG, fw_read_code, CS_FW_VERSION_LENGTH);

    fw_read_version = 0;
    if(ret==0)
    {
        fw_read_version = ((((unsigned short)fw_read_code[1]<<8)&0xff00)|fw_read_code[0]);            
    }

    if(fw_read_version != fw_default_version)
    {
        goto FLAG_FW_FAIL;
    }

    return 0;
    
FLAG_FW_FAIL:

    return (char)-1;
}

/**
  * @brief  firmware high version update
  * @param  *fw_array: point to fw hex array
  * @retval 0:success, -1: fail, 1: no need update
  */
char cs_press_fw_high_version_update(const unsigned char *fw_array)
{
    char ret;
    unsigned char read_temp[FW_ONE_BLOCK_LENGTH_R];
    unsigned short read_version = 0;
    unsigned short default_version = 0;
    char flag_update = 0;   // 0: no need update fw, 1: need update fw
    unsigned char retry;

    cs_press_delay_ms(300); // skip boot jump time

    // read ap version
    ret = cs_press_iic_read(AP_VERSION_REG, read_temp, CS_FW_VERSION_LENGTH);
    
    if(ret == 0)
    {
        // get driver ap version 
        default_version = ((((unsigned short)fw_array[FW_ADDR_VERSION+0]<<8)&0xff00)|fw_array[FW_ADDR_VERSION+1]);

        // get ic ap version
        read_version = ((((unsigned short)read_temp[1]<<8)&0xff00)|read_temp[0]);

        // compare
        if(read_version != default_version)
        {
           
            flag_update = 1;
        }
    }
    else
    {
        flag_update = 1;
    }
    
    if(flag_update == 0)
    {
        return 1;   // no need update
    }

    // update fw
    retry = RETRY_NUM;

    do
    {
        ret = cs_press_fw_force_update(fw_array);
    }while((ret!=0)&&(retry--));
    

    return ret;
}

typedef enum{
    E_STATE_DRIVER_START,
    E_STATE_DRIVER_LOOP,
}driver_state_e;
#define COUNT_DEBUG_READ 50 /*debug状态寄存器重复读取次数 */
/**
  * @brief  read rawdata
  * @param  None
  * @retval 0:success, -1:fail, 1:not finished
  */
char cs_read_rawdata_bytes(unsigned char *rawdata, unsigned char *rawLen)
{
    char ret = -1;
    static char readNumTimes = 0;
    unsigned char byte_num;

    static driver_state_e state_driver = E_STATE_DRIVER_START;

    switch(state_driver)
    {
        case E_STATE_DRIVER_START:
            
            ret = cs_press_set_debugmode(AP_R_DEBUGDATA_DEBUG_MODE);
            if((char)-1 == ret)
            {
                /* iic读取失败 */
                return -1;
            }
            state_driver = E_STATE_DRIVER_LOOP;
            /* 注意这里不需要break,该函数第一次运行也要把以下的代码执行一遍 */
        case E_STATE_DRIVER_LOOP:
            readNumTimes++;
            
            byte_num = cs_press_get_debugready();
            if(readNumTimes > COUNT_DEBUG_READ)
            {
                
                readNumTimes = 0;
                state_driver = E_STATE_DRIVER_START;
                return -1;
            }

            if((byte_num != 0) && (byte_num <= (AFE_MAX_CH*6)))
            {
                readNumTimes = 0;
                *rawLen = byte_num;
                ret = cs_press_read_debugdata(rawdata, byte_num);
                if(-1 == ret)
                {
                    state_driver = E_STATE_DRIVER_START;
                    return -1;
                }
                ret = cs_press_set_debugready(0);
                if(-1 == ret)
                {
                    state_driver = E_STATE_DRIVER_START;
                    return -1;
                }
                state_driver = E_STATE_DRIVER_START;
                return 0;
            }
            else
            {
                /* 数据not readay */
                return 1;
            }
            break;
        default:
            return -1;
            break;
    }
}



char cs_press_read_rawdata_init(void)
{
    char ret;
    
    cs_press_wakeup_iic();
    
    cs_press_clean_debugmode();
    
    ret = cs_press_set_debugmode(AP_R_RAWDATA_MODE);
    
    return ret;
}

/**
  * @brief  read rawdata 
  * @param  *rawdata: point to sensor data strcut
  * @retval 0:none, -1: fail, >0:vaild data num
  */
char cs_press_read_rawdata(CS_RAWDATA_Def *rawdata)
{
    char ret;
    char i;
    unsigned char data_temp[100];
    unsigned char byte_num;
	unsigned char checksum;

    ret = (char)-1;
    
    byte_num = cs_press_get_debugready();
    
    if(byte_num == AFE_USE_CH*6+1)
    {

        ret = cs_press_read_debugdata(data_temp, byte_num);   
  
        if(ret == 0)
        {
            checksum = 0;
            for(i=0;i<AFE_USE_CH*6;i++)
            {
                checksum += data_temp[i];
            }
            
            if(checksum == data_temp[AFE_USE_CH*6])
            {
				for(i=0;i<AFE_USE_CH;i++)
				{
					rawdata->rawdata[i] = ((((unsigned short)data_temp[i*2+1]<<8)&0xff00)|data_temp[2*i]);
					rawdata->baselinedata[i] = ((((unsigned short)data_temp[i*2+1+2*AFE_USE_CH]<<8)&0xff00)|data_temp[2*i+2*AFE_USE_CH]);
					rawdata->forcedata[i] = ((((unsigned short)data_temp[i*2+1+4*AFE_USE_CH]<<8)&0xff00)|data_temp[2*i+4*AFE_USE_CH]);
				}
               
                ret = 0;
            }
			else
			{
				ret = (char)-1;
			}
        }
    }
	
    cs_press_set_debugready(0);
    
    return ret;
}
