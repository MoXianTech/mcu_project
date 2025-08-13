/**
* @file                hal_storage.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup HAL层
  * @{
  */
/**
 * @defgroup 参数存储
 * @brief 参数存储
 * @details 
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
//standard
#include <string.h>

//driver
#include "csa37f7x_rcc.h"
#include "csa37f7x_flash.h"
#include "csa37f7x_syscfg.h"
//hal
#include "hal_storage.h"
#include "hal_common.h"

#include "user_config.h"
#include "framework_debug.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
const unsigned int ProfileBlockMagicCode = 0x48534C46;
/* Exported functions --------------------------------------------------------*/

TypeDef_ProfileBlock BlockData;

static uint32_t flash_read(uint16_t block, uint16_t offset, uint16_t size, uint8_t *buf);
static uint32_t flash_write(uint16_t block, uint16_t offset, uint16_t size, uint8_t *buf);
static uint8_t flash_erase_sector(uint16_t sectorNo);

/**
  * @fn HAL_StatusTypeDef	hal_storage_init(void)
  * @brief  Storage init.
  * @param None.
  * @return HAL_OK.HAL_ERROR
 */

HAL_StatusTypeDef	hal_storage_init(void)
{
    /*打开isp时钟 */
    rcc_ahb_periph_clock_enable_ctrl(RCC_AHBPeriph_ISP,ENABLE);
    /*Flash ISP编程时钟频率 */
    flash_isp_freq_set(FLASH_ISP_FREQ_8M);

    /*Flash APROM更新使能控制，1：APROM可以被更新；0：APROM不能被更新 */
    flash_update_enable_ctrl(FLASH_AREA_APROM,ENABLE);
    /*Flash 用户配置区更新使能控制，1：使能配置区更新；0：禁止配置区更新 */
    flash_update_enable_ctrl(FLASH_AREA_NVR1,ENABLE);
    /*Flash 进入deepstandby使能信号，1：退出deepstandby模式；0：进入deepstandby模式  */
    flash_standby_enable_ctrl(ENABLE);
    /*Flash LDROM更新使能控制，1：LDROM可以被更新.；0：LDROM不能被更新 */
    flash_update_enable_ctrl(FLASH_AREA_LDROM,ENABLE);
    flash_isp_enable_ctrl(ENABLE);
    return HAL_OK;
}

/**
  * @fn   HAL_StatusTypeDef hal_storage_write(unsigned char ucBlock, unsigned short usOffset, unsigned short usLength, void *pBuf)
  * @brief  write data and crc to flash.
  * @param [in] ucBlock:block number. usOffset:offset in block. usLength:write data size. pBuf:point to data writed.
  * @return HAL_OK.HAL_ERROR
 */
HAL_StatusTypeDef hal_storage_write(unsigned char ucBlock, unsigned short usOffset, unsigned short usLength, void *pBuf)
{

    if(ucBlock > BLOCK_MAX)
    {
        return HAL_ERROR;
    }
    if((usOffset + usLength) > PROFILE_BLOCK_DATA_SIZE)
    {
        return HAL_ERROR;
    }
    memset(&BlockData.PBData, 0xFF, PROFILE_BLOCK_DATA_SIZE);
    flash_read(PROFILE_BLOCK_SEC_START + ucBlock, 0, PROFILE_BLOCK_SEC_SIZE, (unsigned char *)&BlockData);

    memcpy(&BlockData.PBData[usOffset], pBuf, usLength);
    BlockData.PBHeader.MagicCode = ProfileBlockMagicCode;
    BlockData.PBHeader.DataCRC32 = calc_crc32(&BlockData.PBData, PROFILE_BLOCK_DATA_SIZE);

    flash_write(PROFILE_BLOCK_SEC_START + ucBlock, 0, PROFILE_BLOCK_SEC_SIZE, (unsigned char *)&BlockData);

    return HAL_OK;
}

/**
  * @fn   HAL_StatusTypeDef hal_storage_read(unsigned char ucBlock, unsigned short usOffset, unsigned short usLength, void *pBuf)
  * @brief  Read data from flash and check crc.
  * @param [in] ucBlock:block number. usOffset:offset in block. usLength:write data size. pBuf:point to data readed.
  * @return HAL_OK.HAL_ERROR
 */
HAL_StatusTypeDef hal_storage_read(unsigned char ucBlock, unsigned short usOffset, unsigned short usLength, void *pBuf)
{
    if(ucBlock > BLOCK_MAX)
    {
        return HAL_ERROR;
    }
    if((usOffset + usLength) > PROFILE_BLOCK_DATA_SIZE)
    {
        return HAL_ERROR;
    }
    memset(&BlockData, 0xFF, sizeof(BlockData));
    flash_read(PROFILE_BLOCK_SEC_START + ucBlock, 0, PROFILE_BLOCK_SEC_SIZE, (unsigned char *)&BlockData);
    if(BlockData.PBHeader.MagicCode == ProfileBlockMagicCode
    && BlockData.PBHeader.DataCRC32 == calc_crc32(&BlockData.PBData, PROFILE_BLOCK_DATA_SIZE))
    {
        memcpy(pBuf, &BlockData.PBData[usOffset], usLength);
        return HAL_OK;
    }
    return HAL_ERROR;
}


/**
  * @fn   uint8_t flash_erase_sector(uint16_t sectorNo)
  * @brief	flash erase
  * @param [in] sectorNo: sector number. 512 bytes every sector. 2 sector are earsed one time.
  * @return 0 is ok.1 is errror
 */
uint8_t flash_erase_sector(uint16_t sectorNo)
{
    uint8_t ret  = 0;
    uint32_t addr = (uint32_t)(sectorNo) << 9;

    REG_WP_DIS();

    ret = flash_page_erase(addr);
    //printf("reterase = %d",ret);

    REG_WP_EN();

    return ret;
}


/**
  * @fn   uint32_t flash_read(uint16_t block,uint16_t offset,uint16_t size,uint8_t * buf)
  * @brief flash read
  * @param [in] block: block number.offset: offset in block. size: size of buffer. buf: buffer of saving data
  * @return 0 is ok.1 is errror
 */
uint32_t flash_read(uint16_t block, uint16_t offset, uint16_t size, uint8_t *buf)
{
    uint32_t ret ;
    uint32_t addr = ( (uint32_t)(block) << 9 ) + offset;

    ret = 0;
    while(size)
    {
        ret |= flash_word_read(addr,(uint32_t *)buf);

        addr += 4;                           //!< Go to next Word
        buf += 4;
        size  -= 4;
    }

    return ret;
}
/**
  * @fn   uint32_t flash_write(uint16_t block,uint16_t offset,uint16_t size,uint8_t * buf)
  * @brief flash write
  * @param [in] block: block number.offset: offset in block. size: size of buffer. buf: buffer of writing data
  * @return 0 is ok.1 is errror
 */
uint32_t flash_write(uint16_t block, uint16_t offset, uint16_t size, uint8_t *buf)
{
    uint32_t ret;
    uint32_t addr = ((uint32_t)(block) << 9) + offset ;

    ret = flash_erase_sector(block);

    while(size)
    {
        ret |= flash_word_program(addr,*(uint32_t *)buf);
        addr += 4;                           //!< Go to next Word
        buf += 4;
        size -= 4;
    }
    return ret;

}

#ifdef __cplusplus
}
#endif
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
